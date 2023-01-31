//

// Created by Daniel on 2023/01/23.
//

#include "dcl_fsm_triKont.h"
/*
void state_triC_create(void *argstr) {
    int (*argstr[state_init])() = state_triC_init;
}
*/

state_triC state_triC_init(state_triC_cluster_type *cluster_in) {
    printf("Init\n");
    dcl_triC_getSetup(cluster_in->device_in);
    /* You can do disgusting things in C, example below */
    if ( !(*(dcl_triC_status *)(cluster_in->device_in->dev_status)).initialised ) {
        dcl_triC_init(cluster_in->device_in);
    }
    dcl_triC_setTopV(cluster_in->device_in, 11);
    return state_idle;
}

state_triC state_triC_idle(state_triC_cluster_type *cluster_in) {
    printf("Idle\n");
    return state_getMsg;
}

state_triC state_triC_getMsg(state_triC_cluster_type* cluster_in) {
    printf("Getting Msg\n");
    if (cluster_in->queue->length) {
        triC_readMsg(cluster_in->queue, &cluster_in->msg_buffer);
        printf("Got Msg: %s\n", cluster_in->msg_buffer.argstr);
        sscanf(cluster_in->msg_buffer.argstr, "%[A-Z],%d,%d",
               cluster_in->nxt_cmd,
               &cluster_in->arg1,
               &cluster_in->arg2);
        return state_action;
    }
    printf("No Msg\n");
    return state_exit;
}

state_triC state_triC_action(state_triC_cluster_type *cluster_in) {
    printf("Executing MSG:\n");
    if (!strcmp("PSH", cluster_in->nxt_cmd)) {
        state_triC_transient(cluster_in);
        dcl_triC_setValve(cluster_in->device_in, cluster_in->arg1);
        state_triC_transient(cluster_in);
        dcl_triC_dispense(cluster_in->device_in, cluster_in->arg2);
    } else if (!strcmp("PUL", cluster_in->nxt_cmd)) {
        state_triC_transient(cluster_in);
        dcl_triC_setValve(cluster_in->device_in, cluster_in->arg1);
        state_triC_transient(cluster_in);
        dcl_triC_aspirate(cluster_in->device_in, cluster_in->arg2);
    }

    return state_getMsg;
}

    state_triC state_triC_transient(state_triC_cluster_type *cluster_in) {
        printf("Transient:\n");
        bool transient = 0;
        do {
            dcl_triC_getStatus(cluster_in->device_in);
            if ((*(dcl_triC_status *) (cluster_in->device_in->dev_status)).statusByte == '@') {
                sleep(1);
                transient = true;
            } else {
                transient = false;
            }
        } while ( transient );
        return state_action;
    }