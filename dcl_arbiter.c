/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 9/19/23. */

#include "dcl_arbiter.h"

arb_cluster  *state_arbFsm_setup(dcl_queue_type *arb_queue_in, dcl_queue_type *tric_queue_in) {
    static dcl_fsm_cluster arb_fsm_cluster;
    arb_fsm_cluster.queue = arb_queue_in;
    static dcl_fsm_cluster pump_cluster;
    pump_cluster.queue = tric_queue_in;

    static arb_cluster thread_cluster;
    thread_cluster.arb_fsm = &arb_fsm_cluster;
    thread_cluster.triC_fsm = &pump_cluster;

    return &thread_cluster;

}

state_arb state_arbFsm_init(arb_cluster *cluster_in) {
    printf("Arbiter Init:\n");

    return state_arb_exit;
}

state_arb state_arbFsm_idle(arb_cluster *cluster_in) {
    /* Is there a message ready? */
    if ( cluster_in->arb_fsm->opt_field & MSGRDY ) {
        /* Read MSG and store */
        return state_arb_readMsg;
    }
    return state_arb_getMsg;
}

state_arb state_arbFsm_getMsg(arb_cluster *cluster_in) {
    int ret_flag = dcl_fsm_getMsg(cluster_in->arb_fsm);

    switch (ret_flag) {
        case MSGEMP:
            return state_arb_getMsg;
        case FSMEND:
            return state_arb_terminate;
        case MSGRET:
            cluster_in->arb_fsm->opt_field |= MSGRDY;
            return state_arb_idle;
        case NOMSGS:
            return state_arb_idle;
        case MSGERR:
        default:
            return state_arb_critical;
    }
}

state_arb state_arbFsm_readMsg(arb_cluster *cluster_in) {
    char *argv[DCL_DCODE_ARGNUM];

    int argc = dcl_sstr_strsep(argv, cluster_in->arb_fsm->msg_buf.argstr,
                               ',',
                               false,
                               DCL_DCODE_ARGNUM,
                               DCL_STRMSG_LEN);

    if (argc == 0) {
        /* TODO: Handle this error */
        printf("Malformed MSG to arbiter\n");
        return state_arb_idle;
    }

    return state_arb_idle;
}

arb_msgList *arb_enqueue(arb_cluster *cluster_in) {
    arb_msgList *p = NULL;
    p = malloc( sizeof (arb_msgList) );
    if (!p) {
        // TODO handle this abort better
        perror("malloc failed for enqueue");
        abort();
    }

    if ( !cluster_in->exec_queue.first ) {
        cluster_in->exec_queue.first = p;
    } else {
        arb_msgList *current = cluster_in->exec_queue.first;
        while (current->next) {
            current = current->next;
        }
        current->next = p;
        cluster_in->exec_queue.last = p;
    }
    cluster_in->exec_queue.length++;
    return p;
}

arb_msgList *arb_dequeue(arb_cluster *cluster_in) {
    arb_msgList  *p = NULL;

    if ( !cluster_in->exec_queue.length ) {
        return NULL;
    } else if ( cluster_in->exec_queue.length == 1) {
        p = cluster_in->exec_queue.first;
        cluster_in->exec_queue.first = NULL;
        cluster_in->exec_queue.last = NULL;
        cluster_in->exec_queue.length = 0;
        return p;
    }

    arb_msgList *current = cluster_in->exec_queue.first;
    while (current->next->next) {
        current = current->next;
    }
    cluster_in->exec_queue.last = current;
    p = current->next;
    current->next = NULL;
    cluster_in->exec_queue.length--;
    return p;
}