/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2023/01/23. */

#include "dcl_threads.h"
extern dcl_triC_status global_pump_array[DCL_TRIC_PUMPNO];
extern pthread_mutex_t status_mutex;
extern dcl_queue_type pump_queue;
extern dcl_queue_type arb_queue;

void *pumpThread(void *arg) {
    /* Finite state machine array */
    state_triC ( *fsm_triC[numStates] )(triC_fsm_cluster *cluster_in) =
            {[state_idle] = state_triC_idle,
             [state_init] = state_triC_init,
             [state_getMsg] = state_triC_getMsg,
             [state_supervise] = state_triC_supervise,
             [state_delegate] = state_triC_delegate,
             [state_resolve] = state_triC_resolve,
             [state_action] = state_triC_action,
             [state_transient] = state_triC_transient,
             [state_halt] = state_triC_halt,
             [state_critical] = state_triC_critical,
             [state_terminate] = state_triC_terminate};
    state_triC next_state = state_init;

    int addr_array[DCL_TRIC_PUMPNO] = {0, 1};
    dcl_serialDevice_triC *dev_trikC3000 = dcl_triC_multiSetup("TriKont Syringe",
                                                               (char *)arg,
                                                         addr_array);
    if (!dev_trikC3000) {
        abort();
    }

    triC_fsm_cluster *thread_cluster = state_triC_fsmSetup(&pump_queue,
                                                          dev_trikC3000,
                                                          &status_mutex,
                                                          global_pump_array);


    while (next_state != state_exit) {
        next_state = fsm_triC[next_state](thread_cluster);
    }

    dcl_triC_destroy(dev_trikC3000);
    return 0;
}

void *parserThread(void *arg) {
    /* Finite state machine array */
    state_dcode ( *fsm_dcode[dcode_numstates] )(dcode_cluster *cluster_in) =
            {[state_dcode_init] = state_dcodeM_init,
             [state_dcode_scan] = state_dcodeM_scan,
             [state_dcode_blkStart] = state_dcodeM_blkStart,
             [state_dcode_blkEnd] = state_dcodeFsm_blkEnd,
             [state_dcode_config] = state_dcodeFsm_config,
             [state_dcode_step] = state_dcodeFsm_step,
             [state_dcode_run] = state_dcodeFsm_run,
            [state_dcode_abort] = state_dcodeFsm_abort};
    state_dcode next_state = state_dcode_init;

    dcode_cluster *thread_cluster = state_dcodeM_setup( arg, &arb_queue);

    while (next_state != state_dcode_exit) {
        next_state = fsm_dcode[next_state](thread_cluster);
    }

    fclose(thread_cluster->file.file_pointer);
    return 0;
}

void *arbThread(void *arg) {
    /* Finite state machine array */
    state_spv (*fsm_arb[arb_numStates] )(spv_cluster * cluster_in) =
            {[state_spv_init] = state_spvFsm_init,
             [state_spv_idle] = state_spvFsm_idle,
             [state_spv_getJob] = state_spvFsm_getJob,
             [state_arb_readMsg] = state_spvFsm_procJob};

    state_spv next_state = state_spv_init;
    spv_cluster *thread_cluster = state_spvFsm_setup(&arb_queue, &pump_queue);

    while (next_state != state_arb_exit) {
        thread_cluster->prev_state = next_state;
        next_state = fsm_arb[next_state](thread_cluster);
    }

    return 0;
}