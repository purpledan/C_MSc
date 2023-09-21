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