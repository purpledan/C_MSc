/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2023/01/23*/

#include "dcl_fsm_triKont.h"

triC_fsm_cluster *state_triC_fsmSetup(dcl_queue_type *fsm_msg_queue,
                                      dcl_serialDevice_triC *triC_dev,
                                      pthread_mutex_t *ext_mutex,
                                      dcl_triC_status *ext_status) {
    static dcl_fsm_cluster fsm_cluster;
    fsm_cluster.queue = fsm_msg_queue;

    static triC_fsm_cluster thread_cluster;
    thread_cluster.fsm = &fsm_cluster;
    thread_cluster.device_in = triC_dev;
    thread_cluster.init_complete = false;

    static triC_fsm_buf message_buf[DCL_TRIC_PUMPNO];
    thread_cluster.cmd_array = message_buf;

    printf("Checking FSM Cluster\n");
    for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
        thread_cluster.cmd_array[i].addr_arg = thread_cluster.device_in->dev_status_array[i].address;
        thread_cluster.cmd_array[i].dev_flags = '\0';
        printf("Address %d is %d\n", i, thread_cluster.device_in->dev_status_array[i].address);
    }

    fsm_cluster.opt_field = '\0';

    if (ext_status) {
        thread_cluster.enable_external = true; // Link to ext_array status available
        thread_cluster.ext_array = ext_status;
        thread_cluster.ext_mutex = ext_mutex;
    } else {
        thread_cluster.enable_external = false;
    }
    return &thread_cluster;
}

state_triC state_triC_init(triC_fsm_cluster *cluster_in) {
    printf("Init:\n");

    int ret_flag;

    dcl_triC_multiGetSetup(cluster_in->device_in);

    for (size_t i = 0; i < DCL_TRIC_PUMPNO; i++ ) {

        cluster_in->device_in->dev_select = i;
        // dcl_triC_init(cluster_in->device_in);

        if (!cluster_in->device_in->dev_status_array[i].initialised) {
            dcl_triC_init(cluster_in->device_in);
            //sleep(1); // Needed for low power-supply
        }

    }
    sleep(7);       // TODO: Add way so that state_transient is used instead

    ret_flag = ext_triC_updateStatus(cluster_in);
    if (!ret_flag) {
        cluster_in->init_complete = true;
    }
    return state_getMsg;
}

state_triC state_triC_idle(triC_fsm_cluster *cluster_in) {
    /* Is there a message ready for us? */
    if ( cluster_in->fsm->opt_field & MSGRDY ) {
        /* Read Msg and perform action */
        return state_supervise;
    } else {
        /* There is no MSG in the buffer ready */
        if (cluster_in->fsm->queue_empty) {
            /* but there is an action ready */
            if (cluster_in->fsm->opt_field & ACTRDY) {
                /* Delegate action */
                return state_delegate;
            } else {
                /* Queue is empty, we goto wait*/
                return state_halt;
            }
        } else {
            /* We get a new msg from the queue */
            return state_getMsg;
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

state_triC state_triC_supervise(triC_fsm_cluster *cluster_in) {
    if ( cluster_in->fsm->opt_field & (BUFCOL | SYNCRO) ) {
        /* Either buffer collision or synchronisation mode */
        /* Skip all other handling tasks */
    } else {
        /* There was no collision previously, so we go on as normal */
        /* Parse and store MSG in staging buffer */
        int special = aux_triC_parseMsg(cluster_in);
        /* Signal that MSG has been read from queue */
        cluster_in->fsm->opt_field &= ~MSGRDY;
        if (special) {
            if (cluster_in->cmd_buf.nxt_cmd == action_syn) {
                cluster_in->fsm->opt_field |= SYNCRO;
                return state_delegate;
            } else if (cluster_in->cmd_buf.nxt_cmd == action_nan) {
                return state_delegate;
            }
        }
        /* Attempt to store MSG in dev buffer */
        int addr = cluster_in->cmd_buf.addr_arg;
        if ( !(cluster_in->cmd_array[addr].dev_flags & BUFFUL) ) {
            cluster_in->cmd_array[addr].nxt_cmd = cluster_in->cmd_buf.nxt_cmd;
            cluster_in->cmd_array[addr].arg0 = cluster_in->cmd_buf.arg0;
            cluster_in->cmd_array[addr].arg1 = cluster_in->cmd_buf.arg1;
            cluster_in->cmd_array[addr].arg2 = cluster_in->cmd_buf.arg2;
            cluster_in->cmd_array[addr].dev_flags |= BUFFUL;
        } else {
            /* Buffer collision! */
            cluster_in->fsm->opt_field |= MSGRDY;           // We don't want to write over the current message yet
            cluster_in->cmd_array[addr].dev_flags |= BUFCOL;// Flag the specific dev
            cluster_in->fsm->opt_field |= BUFCOL;           // Flag the overall status too
            return state_idle;
        }
    }
    return state_delegate;
}

state_triC state_triC_delegate(triC_fsm_cluster *cluster_in) {
    /* If the pump is busy, we can't perform the action */
    /* Pump(s) in transient, get status of all SPs */
    dcl_triC_multiGetStatus(cluster_in->device_in);
    /* Also updates the global flag if any single SP is busy */
    aux_triC_updateBusyStatus(cluster_in);
    /* Flag ACTRDY if a buffer is full and ready for action */
    aux_triC_updateActionStatus(cluster_in);

    bool perform_task = false;
    /* If we are in SYNCRO mode, we halt all SP actions until last action is complete */
    /* However we can only attempt SYNCRO when there are no buffer collisions */
   if (cluster_in->fsm->opt_field & ACTRDY) {
       for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
           /* Exclude collided pump in check */
           if (cluster_in->cmd_array[i].dev_flags & BUFCOL) {
               if ( !(cluster_in->cmd_array[i].dev_flags & SPBUSY) ) {
                   perform_task = true;
                   cluster_in->device_in->dev_select = i;
                   /* Reset device status */
                   cluster_in->cmd_array[i].dev_flags &= ~BUFCOL;
                   cluster_in->fsm->opt_field &= ~BUFCOL;
               } else {
                   continue;
               }
           }
           if (cluster_in->cmd_array[i].dev_flags & BUFFUL) {
               if ( !(cluster_in->cmd_array[i].dev_flags & SPBUSY) ) {
                   perform_task = true;
                   cluster_in->device_in->dev_select = i; // TODO: Possible error with device address
                   break;
               }
           }
       }
   } else if ( !(cluster_in->fsm->opt_field & SPBUSY) ) {
       cluster_in->fsm->opt_field &= ~SYNCRO;
   }

   if (perform_task) {
       cluster_in->fsm->opt_field &= ~RETARD;
       return state_action;
   } else {
       cluster_in->fsm->opt_field |= RETARD;
       return state_transient;
   }
}

state_triC state_triC_resolve(triC_fsm_cluster *cluster_in) {
    /* Attempt to resolve buffer collision */
    dcl_triC_multiGetStatus(cluster_in->device_in);
    aux_triC_updateBusyStatus(cluster_in);
    aux_triC_updateActionStatus(cluster_in);

    bool perform_task = false;
    for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
        /* Look for blocking pump */
        if (cluster_in->cmd_array[i].dev_flags & BUFCOL) {
            /* Is it busy? */
            if ( !(cluster_in->cmd_array[i].dev_flags & SPBUSY) ) {
                perform_task = true;
                cluster_in->device_in->dev_select = i;
                /* Reset device status */
                cluster_in->cmd_array[i].dev_flags &= ~BUFCOL;
            } else {
                /* The pump is still blocking, ask delegator if another pump is ready */
                return state_delegate;
            }
            break;
        }
    }
    if (perform_task) {
        /* Reset buffer collision status since the next action will resolve the problem */
        cluster_in->fsm->opt_field &= ~BUFCOL;
        cluster_in->fsm->opt_field &= ~RETARD;
        return state_action;
    } else {
        cluster_in->fsm->opt_field |= RETARD;
        return state_transient;
    }
}

state_triC state_triC_action(triC_fsm_cluster *cluster_in) {
    dcl_printTime();
    printf("Performing: ");
    switch (cluster_in->cmd_array[cluster_in->device_in->dev_select].nxt_cmd) {
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
        case action_syn:
            action_triC_syn(cluster_in);
            break;
        case action_err:
        default:
            printf("CMD made no sense\n");
            return state_critical;
    }
    /* At this point the buffer has been read, clear the flag */
    cluster_in->cmd_array[cluster_in->device_in->dev_select].dev_flags &= ~BUFFUL;

    return state_idle;
}

state_triC state_triC_transient(triC_fsm_cluster *cluster_in) {
    /* We may use this downtime to update the ext_array status */
    ext_triC_updateStatus(cluster_in);

    if (cluster_in->fsm->opt_field & RETARD) {
        /* Slow down mechanism so that we don't consume a billion CPU cycles */
        struct timespec halt = {
                .tv_nsec = 128000000,   // 128ms?
        };
        nanosleep(&halt, NULL);
    }
    return state_idle;
}

state_triC state_triC_halt(triC_fsm_cluster *cluster_in) {
    int status = pthread_mutex_lock(&cluster_in->fsm->queue->mutex);
    if (status) {
        perror("Queue Halt Failure: ");
        return state_critical;
    }

    if ( cluster_in->fsm->queue_empty ) {
        printf("Queue Empty, halting\n");
        pthread_cond_wait(&cluster_in->fsm->queue->alert,
                          &cluster_in->fsm->queue->mutex);
        /* If a spurious wakeup occurs, the fsm will test the queue again
         * if it is still empty, this state will execute again continuing
         * in a loop until a new message is received */
        cluster_in->fsm->queue_empty = false;
    }
    pthread_mutex_unlock(&cluster_in->fsm->queue->mutex);
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
            /* Check the overall status of all the pumps */
            /* Grab the mutex lock for the entire loop */
            ret_flag = pthread_mutex_lock(cluster_in->ext_mutex);
            for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
                cluster_in->ext_array[i].initialised = cluster_in->device_in->dev_status_array[i].initialised;
                cluster_in->ext_array[i].statusByte = cluster_in->device_in->dev_status_array[i].statusByte;
                cluster_in->ext_array[i].topV = cluster_in->device_in->dev_status_array[i].topV;
                cluster_in->ext_array[i].plunger = cluster_in->device_in->dev_status_array[i].plunger;
                cluster_in->ext_array[i].valve = cluster_in->device_in->dev_status_array[i].valve;
            }
            pthread_mutex_unlock(cluster_in->ext_mutex);
        } else {
            ret_flag = pthread_mutex_trylock(cluster_in->ext_mutex);
            if (ret_flag) {
                for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
                    cluster_in->ext_array[i].statusByte = cluster_in->device_in->dev_status_array[i].statusByte;
                    cluster_in->ext_array[i].topV = cluster_in->device_in->dev_status_array[i].topV;
                    cluster_in->ext_array[i].plunger = cluster_in->device_in->dev_status_array[i].plunger;
                    cluster_in->ext_array[i].valve = cluster_in->device_in->dev_status_array[i].valve;
                }
                pthread_mutex_unlock(cluster_in->ext_mutex);
            }
        }
    } else {
        ret_flag = 0;
    }
    return ret_flag;
}

int aux_triC_parseMsg(triC_fsm_cluster *cluster_in) {
    char temp_string[8] = "";

    if ( !isdigit(cluster_in->fsm->msg_buf.argstr[0]) ) {
        char temp_arg[32] = "";
        sscanf(cluster_in->fsm->msg_buf.argstr, "!,%[A-Z],%[A-Za-z0-9-]",
        temp_string,
        temp_arg);
        if (!strcmp("SYN", temp_string)) {
            cluster_in->cmd_buf.nxt_cmd = action_syn;
        } else if (!strcmp("MSG", temp_string)) {
            printf("MSG: %s\n", temp_arg);
            cluster_in->cmd_buf.nxt_cmd = action_nan;
        }
        return 1;
    }

    sscanf(cluster_in->fsm->msg_buf.argstr, "%d,%[A-Z],%d,%d,%d",
           &cluster_in->cmd_buf.addr_arg,
           temp_string,
           &cluster_in->cmd_buf.arg0,
           &cluster_in->cmd_buf.arg1,
           &cluster_in->cmd_buf.arg2);

    if (!strcmp("PSH", temp_string)) {
        cluster_in->cmd_buf.nxt_cmd = action_psh;
    } else if (!strcmp("PUL", temp_string)) {
        cluster_in->cmd_buf.nxt_cmd = action_pul;
    } else if (!strcmp("CFG", temp_string)) {
        cluster_in->cmd_buf.nxt_cmd = action_cfg;
    } else if (!strcmp("SET", temp_string)) {
        cluster_in->cmd_buf.nxt_cmd = action_set;
    } else {
        cluster_in->cmd_buf.nxt_cmd = action_err;
    }

    return 0;
}

void aux_triC_updateBusyStatus(triC_fsm_cluster *cluster_in) {
    dcl_triC_multiGetStatus(cluster_in->device_in);
    bool temp_flag = false;
    for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
        if ( cluster_in->device_in->dev_status_array[i].statusByte == '@') {
            cluster_in->cmd_array[i].dev_flags |= SPBUSY;
            temp_flag = true;
        } else {
            cluster_in->cmd_array[i].dev_flags &= ~SPBUSY;
        }
    }
    if (temp_flag) {
        cluster_in->fsm->opt_field |= SPBUSY;
    } else {
        cluster_in->fsm->opt_field &= ~SPBUSY;
    }
}

void aux_triC_updateActionStatus(triC_fsm_cluster *cluster_in) {
    bool temp_flag = false;
    for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
        if ( cluster_in->cmd_array[i].dev_flags & BUFFUL) {
            temp_flag = true;
        }
    }
    if (temp_flag) {
        cluster_in->fsm->opt_field |= ACTRDY;
    } else {
        cluster_in->fsm->opt_field &= ~ACTRDY;
    }
}

void action_triC_psh(triC_fsm_cluster *cluster_in) {
    int addr = (int)cluster_in->device_in->dev_select;
    printf("Pump %d Dispensing %d from valve %d @ %d\n",
                            addr,
                            cluster_in->cmd_array[addr].arg1,
                            cluster_in->cmd_array[addr].arg0,
                            cluster_in->cmd_array[addr].arg2);
    if (cluster_in->cmd_array[addr].arg2 == 0) {
        dcl_triC_dispenseAtomic(cluster_in->device_in,
                                cluster_in->cmd_array[addr].arg0,
                                cluster_in->cmd_array[addr].arg1);
    } else {
        dcl_triC_setV(cluster_in->device_in,
                      cluster_in->cmd_array[addr].arg2);
        dcl_triC_dispenseAtomic(cluster_in->device_in,
                                cluster_in->cmd_array[addr].arg0,
                                cluster_in->cmd_array[addr].arg1);
    }

    cluster_in->cmd_array[addr].dev_flags |= SPBUSY;
}
void action_triC_pul(triC_fsm_cluster *cluster_in) {
    int addr = (int)cluster_in->device_in->dev_select;
    printf("Pump %d Aspirating %d from valve %d @ %d\n",
                             addr,
                             cluster_in->cmd_array[addr].arg1,
                            cluster_in->cmd_array[addr].arg0,
                            cluster_in->cmd_array[addr].arg2);
    if (cluster_in->cmd_array[addr].arg2 == 0) {
        dcl_triC_aspirateAtomic(cluster_in->device_in,
                                cluster_in->cmd_array[addr].arg0,
                                cluster_in->cmd_array[addr].arg1);
    } else {
        dcl_triC_setV(cluster_in->device_in,
                      cluster_in->cmd_array[addr].arg2);
        dcl_triC_aspirateAtomic(cluster_in->device_in,
                                cluster_in->cmd_array[addr].arg0,
                                cluster_in->cmd_array[addr].arg1);
    }

    cluster_in->cmd_array[addr].dev_flags |= SPBUSY;
}

void action_triC_set(triC_fsm_cluster *cluster_in) {
    int addr = (int)cluster_in->device_in->dev_select;
    printf("Pump %d Setting Valve to %d and plunger to %d\n",
           addr,
           cluster_in->cmd_array[addr].arg0,
           cluster_in->cmd_array[addr].arg1);
    dcl_triC_setAtomic(cluster_in->device_in, cluster_in->cmd_array[addr].arg0,
                       cluster_in->cmd_array[addr].arg1);
    cluster_in->cmd_array[addr].dev_flags |= SPBUSY;
}

void action_triC_cfg(triC_fsm_cluster *cluster_in) {
    /* No action, only changing a setting */
    int addr = (int)cluster_in->device_in->dev_select;
    switch (cluster_in->cmd_array[addr].arg0) {
        case 1: //TODO: Remove magic Number
            dcl_triC_setV(cluster_in->device_in, cluster_in->cmd_array[addr].arg1);
            break;
        default:
            break;
    }
}

void action_triC_syn(triC_fsm_cluster *cluster_in) {
    /* No action, only syncing*/
    printf("Synchronisation\n");
    cluster_in->fsm->opt_field |= SYNCRO;
}