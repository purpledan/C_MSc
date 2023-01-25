//
// Created by Daniel on 2023/01/23.
//

#include "dcl_fsm_triKont.h"
/*
void state_triC_create(void *arg) {
    int (*arg[state_init])() = state_triC_init;
}
*/

state_triC state_triC_init(state_triC_cluster_type *cluster_in) {
    dcl_triC_init(cluster_in->device_in);
    return state_idle;
}

state_triC state_triC_idle(state_triC_cluster_type *cluster_in) {
    printf("Leaving FSM\n");
    return state_exit;
}