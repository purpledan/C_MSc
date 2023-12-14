/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 9/19/23. */

#include "dcl_supervisor.h"

spv_cluster  *state_spvFsm_setup(dcl_queue_type *arb_queue_in, dcl_queue_type *tric_queue_in) {
    static dcl_fsm_cluster arb_fsm_cluster;
    arb_fsm_cluster.queue = arb_queue_in;
    static dcl_fsm_cluster pump_cluster;
    pump_cluster.queue = tric_queue_in;

    static spv_cluster thread_cluster;
    thread_cluster.spv_fsm = &arb_fsm_cluster;
    thread_cluster.triC_fsm = &pump_cluster;

    return &thread_cluster;

}

state_spv state_spvFsm_init(spv_cluster *cluster_in) {
    /* Here you should place the initial jobs that get sent to the devices */
    printf("Supervisor Init:\n");
    return state_spv_idle;
}

state_spv state_spvFsm_idle(spv_cluster *cluster_in) {
    /* Here the supervisor needs to check for emergencies */

    /* If there are no emergencies, get back to work */
    switch (cluster_in->prev_state) {
        case state_spv_init:
            /* First responsibility of supervisor */
            return state_spv_term;
        case state_spv_procJob:
            /* Received job, don't check immediately for more jobs */ 


    }
    if (cluster_in->spv_fsm->opt_field & MSGRDY ) {
        /* Read MSG and store */
        return state_arb_readMsg;
    }
    return state_spv_getJob;
}

state_spv state_spvFsm_getJob(spv_cluster *cluster_in) {
    /* Get MSGs from the supervisor input queue */
    int ret_flag = dcl_fsm_getMsg(cluster_in->spv_fsm);
    switch (ret_flag) {
        case MSGEMP:
            return state_spv_getJob;
        case FSMEND:
            return state_spv_term;
        case MSGRET:
            /* A valid msg has been received, so it needs to be processed */
            return state_spv_procJob;
        case NOMSGS:
            return state_arb_exit;  // TODO: Escape here
        case MSGERR:
        default:
            return state_spv_crit;
    }
}

state_spv state_spvFsm_procJob(spv_cluster *cluster_in) {
    char *argv[DCL_DCODE_ARGNUM];
    int argc = dcl_sstr_strsep(argv, cluster_in->spv_fsm->msg_buf.argstr,
                               ',',
                               false,
                               DCL_DCODE_ARGNUM,
                               DCL_STRMSG_LEN);
    if (argc == 0) {
        /* TODO: Implement a global error system */
        fprintf(stderr,"Malformed job handed to supervisor:\n");
        return state_spv_crit;
    }
    job_msg *p = arb_enqueue(cluster_in);
    /* The first arg is special, we change this into an int */
    p->dest_ID = (int) strtol(argv[0], NULL, 10);
    /* Since we have used the first arg, we start counting from 1 inside argv[]*/
    for (int i = 0; i < argc - 1; i++) {
        strcpy(p->msg[i], argv[i + 1]);
    }


    return state_spv_idle;
}

state_spv state_arbFsm_procMsg(spv_cluster *cluster_in) {
    job_msg *p = arb_dequeue(cluster_in);
    state_spv ret_val;
    /* This MSG now needs to go to the dest_ID */
    if (p->dest_ID == 0) {
        /* The MSG is for me, yay! */
        ret_val = state_spv_idle;
    } else if (p->dest_ID == 1) {
        /* This MSG has to go to the pumps */
        ret_val = state_arb_sendMsg;
    } else {
        /* Wrong dest_ID */
        ret_val = state_spv_crit;
    }
    free(p);
    return ret_val;
}

job_msg *arb_enqueue(spv_cluster *cluster_in) {
    job_msg *p = NULL;
    p = malloc( sizeof (job_msg) );
    if (!p) {
        // TODO handle this abort better
        perror("malloc failed for enqueue");
        abort();
    }

    if ( !cluster_in->staging_queue.length ) {
        cluster_in->staging_queue.first = p;
        cluster_in->staging_queue.last = p;
    } else {
        cluster_in->staging_queue.last->next = p;
        cluster_in->staging_queue.last = p;
    }
    p->next = NULL;
    cluster_in->staging_queue.length++;
    return p;
}

job_msg *arb_dequeue(spv_cluster *cluster_in) {
    job_msg  *p = NULL;

    if ( !cluster_in->staging_queue.length ) {
        return NULL;
    } else if (cluster_in->staging_queue.length == 1) {
        p = cluster_in->staging_queue.first;
        cluster_in->staging_queue.first = NULL;
        cluster_in->staging_queue.last = NULL;
        cluster_in->staging_queue.length = 0;
        return p;
    }

    p = cluster_in->staging_queue.first;
    cluster_in->staging_queue.first->next = cluster_in->staging_queue.first;

    cluster_in->staging_queue.length--;
    return p;
}