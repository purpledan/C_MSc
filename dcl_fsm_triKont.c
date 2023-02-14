//

// Created by Daniel on 2023/01/23.
//

#include "dcl_fsm_triKont.h"
/*
void state_triC_fsmCreate(void *argstr) {
    int (*argstr[state_init])() = state_triC_init;
}
*/

triC_fsm_cluster *state_triC_fsmSetup(dcl_queue_type *fsm_msg_queue,
                                      dcl_serialDevice *triC_dev,
                                      pthread_mutex_t *ext_mutex,
                                      dcl_triC_status *ext_status) {
    static dcl_fsm_cluster_type fsm_cluster;
    fsm_cluster.queue = fsm_msg_queue;

    static triC_fsm_cluster thread_cluster;
    thread_cluster.fsm = &fsm_cluster;
    thread_cluster.device_in = triC_dev;
    thread_cluster.init_complete = false;

    thread_cluster.status_in = ( (dcl_triC_status *)(triC_dev->dev_status) );
    printf("Checking FSM Cluster\n");
    printf("Pump Address is %d\n", thread_cluster.status_in->address);

    fsm_cluster.opt_field = '\0';

    if (ext_status) {
        thread_cluster.enable_external = true; // Link to external status available
        thread_cluster.external = ext_status;
        thread_cluster.ext_mutex = ext_mutex;
    } else {
        thread_cluster.enable_external = false;
    }
    return &thread_cluster;
}

state_triC state_triC_init(triC_fsm_cluster *cluster_in) {
    printf("Init:\n");

    int ret_flag;
    dcl_triC_getSetup( cluster_in->device_in);
    if ( !cluster_in->status_in->initialised ) {
        dcl_triC_init(cluster_in->device_in);
        sleep(7);       // TODO: Add way so that state_transient is used instead
    }

    ret_flag = ext_triC_updateStatus(cluster_in);
    if (!ret_flag) {
        cluster_in->init_complete = true;
    }
    return state_getMsg;
}

state_triC state_triC_idle(triC_fsm_cluster *cluster_in) {
    if ( cluster_in->fsm->opt_field & ACTBSY ) {
        /* An action is being performed */
        if ( cluster_in->fsm->opt_field & SPBUSY ) {
            /* and the pump is busy */
            return state_transient;
        }
        return state_action;
    } else {
        /* There is no action being performed */
        /* Is there a message ready for us? */
        if ( cluster_in->fsm->opt_field & MSGRDY ) {
            /* Read Msg and perform action */
            return state_action;
        } else {
            /* There is no MSG in the buffer ready */
            if (cluster_in->fsm->queue_empty) {
                /* Queue is empty, we goto wait TODO: add wait state*/
                return state_idle;
            } else {
                /* We get a new msg from the queue */
                return state_getMsg;
            }
        }
    }
}

state_triC state_triC_getMsg(triC_fsm_cluster *cluster_in) {
    int ret_flag = dcl_fsm_thr_getMsg(cluster_in->fsm);

    switch (ret_flag) {
        case MSGEMP:
            return state_getMsg;
        case FSMEND:
            return state_terminate;
        case MSGRET:
            cluster_in->fsm->opt_field |= MSGRDY;
            return state_idle;
        case NOMSGS:
            return state_idle;
        case MSGERR:
        default:
            return state_critical;
    }
}

state_triC state_triC_action(triC_fsm_cluster *cluster_in) {
    printf("Performing: ");
    /* If there is no action being performed, we start a new one */
    if ( !(cluster_in->fsm->opt_field & ACTBSY) ) {
        cluster_in->fsm->opt_field |= ACTBSY;
        aux_triC_parseMsg(cluster_in);
        cluster_in->fsm->opt_field &= ~MSGRDY;
        /* It is only logical that the valve be at the correct
         * position before the plunger starts moving, it might
         * be logical that the plunger moves before the correct
         * valve position, but such logic breaks the rules of
         * causality
         * */
        cluster_in->fsm->opt_field |= ARGSEL;
        cluster_in->fsm->opt_field &= ~LSTACT;
    }

    dcl_triC_getStatus(cluster_in->device_in);
    /* At this point an action is surely being performed */
    switch (cluster_in->nxt_cmd) {
        case action_pul:
            action_triC_pul(cluster_in);
            break;
        case action_psh:
            action_triC_psh(cluster_in);
            break;
        case action_cfg:
            action_triC_cfg(cluster_in);
            break;
        case action_set:
            action_triC_set(cluster_in);
            break;
        case action_err:
        default:
            printf("CMD made no sense\n");
            return state_critical;
    }

    return state_idle;
}

state_triC state_triC_transient(triC_fsm_cluster *cluster_in) {
    //int ret_flag;

    /* Pump in transient, get status of pump */
    dcl_triC_getStatus(cluster_in->device_in);
    if ( cluster_in->status_in->statusByte == '@' ) {
        cluster_in->fsm->opt_field |= SPBUSY;
    } else {
        cluster_in->fsm->opt_field &= ~SPBUSY;
    }

    if ( (cluster_in->fsm->opt_field & LSTACT) && !(cluster_in->fsm->opt_field & SPBUSY) ) {
        cluster_in->fsm->opt_field &= ~ACTBSY;
    }

    /* We may use this downtime to update the external status */
    ext_triC_updateStatus(cluster_in);

    /* Slow down mechanism so that we don't consume a billion CPU cycles */
    struct timespec halt = {
            .tv_nsec = 128000000,   // 128ms?
    };
    nanosleep(&halt, NULL);

    return state_idle;
}

state_triC state_triC_critical(triC_fsm_cluster *cluster_in) {
    fprintf(stderr, "Critical pump FSM failure\n");
    abort();
}

state_triC state_triC_terminate(triC_fsm_cluster *cluster_in) {
    printf("Terminating Pump FSM:\n");
    return state_exit;
}

int ext_triC_updateStatus(triC_fsm_cluster *cluster_in) {
    int ret_flag;
    if ( cluster_in->enable_external ) {
        if ( !(cluster_in->init_complete) ) {
            ret_flag = pthread_mutex_lock(cluster_in->ext_mutex);
            cluster_in->external->initialised = cluster_in->status_in->initialised;
            cluster_in->external->statusByte = cluster_in->status_in->statusByte;
            cluster_in->external->topV = cluster_in->status_in->topV;
            cluster_in->external->plunger = cluster_in->status_in->plunger;
            cluster_in->external->valve = cluster_in->status_in->valve;
            pthread_mutex_unlock(cluster_in->ext_mutex);
        } else {
            ret_flag = pthread_mutex_trylock(cluster_in->ext_mutex);
            cluster_in->external->initialised = cluster_in->status_in->initialised;
            cluster_in->external->statusByte = cluster_in->status_in->statusByte;
            cluster_in->external->topV = cluster_in->status_in->topV;
            cluster_in->external->plunger = cluster_in->status_in->plunger;
            cluster_in->external->valve = cluster_in->status_in->valve;
            if (!ret_flag) {
                pthread_mutex_unlock(cluster_in->ext_mutex);
            }
        }
    } else {
        ret_flag = 0;
    }
    return ret_flag;
}

void aux_triC_parseMsg(triC_fsm_cluster *cluster_in) {
    char temp_string[8] = "";
    // TODO: Use strtol
    sscanf(cluster_in->fsm->msg_buf.argstr, "%[A-Z],%d,%d",
           temp_string,
           &cluster_in->arg1,
           &cluster_in->arg2);

    if (!strcmp("PSH", temp_string)) {
        cluster_in->nxt_cmd = action_psh;
    } else if (!strcmp("PUL", temp_string)) {
        cluster_in->nxt_cmd = action_pul;
    } else if (!strcmp("CFG", temp_string)) {
        cluster_in->nxt_cmd = action_cfg;
    } else if (!strcmp("SET", temp_string)) {
        cluster_in->nxt_cmd = action_set;
    } else {
        cluster_in->nxt_cmd = action_err;
    }
}

void action_triC_sel(triC_fsm_cluster *cluster_in) {
    printf("Valve move to: %d\n", cluster_in->arg1);
    dcl_triC_setValve(cluster_in->device_in, cluster_in->arg1);
    cluster_in->fsm->opt_field &= ~ARGSEL;
}

void action_triC_psh(triC_fsm_cluster *cluster_in) {
    if ( cluster_in->fsm->opt_field & ARGSEL ) {
        action_triC_sel(cluster_in);
    } else {
        cluster_in->fsm->opt_field |= LSTACT;
        printf("Dispensing %d\n", cluster_in->arg2);
        dcl_triC_dispense(cluster_in->device_in, cluster_in->arg2);
    }
    /* The SP should be busy moving now */
    cluster_in->fsm->opt_field |= SPBUSY;
}
void action_triC_pul(triC_fsm_cluster *cluster_in) {
    if ( cluster_in->fsm->opt_field & ARGSEL ) {
        action_triC_sel(cluster_in);
    } else {
        cluster_in->fsm->opt_field |= LSTACT;
        printf("Aspirating %d\n", cluster_in->arg2);
        dcl_triC_aspirate(cluster_in->device_in, cluster_in->arg2);
    }
    /* The SP should be busy moving now */
    cluster_in->fsm->opt_field |= SPBUSY;

}

void action_triC_set(triC_fsm_cluster *cluster_in) {
    if ( cluster_in->fsm->opt_field & ARGSEL ) {
        action_triC_sel(cluster_in);
    } else {
        cluster_in->fsm->opt_field |= LSTACT;
        printf("Setting Plunger %d\n", cluster_in->arg2);
        dcl_triC_setPlunger(cluster_in->device_in, cluster_in->arg2);
    }
    /* The SP should be busy moving now */
    cluster_in->fsm->opt_field |= SPBUSY;

}

void action_triC_cfg(triC_fsm_cluster *cluster_in) {
    /* No action, only changing a setting */
    cluster_in->fsm->opt_field &= ~ACTBSY;
    switch (cluster_in->arg1) {
        case setting_speed:
            dcl_triC_setSpeed(cluster_in->device_in, cluster_in->arg2);
            break;
        default:
            break;
    }
}