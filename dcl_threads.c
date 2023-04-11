//
// Created by Daniel on 2023/01/23.
//

#include "dcl_threads.h"
extern dcl_triC_status pump_status;
extern pthread_mutex_t status_mutex;
extern dcl_queue_type worker_queue;

void *pumpThread(void *arg) {
    /* Finite state machine array */
    state_triC ( *fsm_triC[numStates] )(triC_fsm_cluster *cluster_in) =
            {[state_idle] = state_triC_idle,
             [state_init] = state_triC_init,
             [state_getMsg] = state_triC_getMsg,
             [state_supervise] = state_triC_supervise,
             [state_action] = state_triC_action,
             [state_transient] = state_triC_transient,
             [state_halt] = state_triC_halt,
             [state_critical] = state_triC_critical,
             [state_terminate] = state_triC_terminate};
    state_triC next_state = state_init;

    int addr_array[DCL_TRIC_PUMPNO] = {0, 1, 2, 3};
    dcl_serialDevice_triC *dev_trikC3000 = dcl_triC_multiSetup("TriKont Syringe",
                                                         "/dev/ttyUSB0",
                                                         addr_array);
    if (!dev_trikC3000) {
        abort();
    }

    triC_fsm_cluster *thread_cluster = state_triC_fsmSetup(&worker_queue,
                                                          dev_trikC3000,
                                                          &status_mutex,
                                                          &pump_status);


    while (next_state != state_exit) {
        next_state = fsm_triC[next_state](thread_cluster);
    }

    dcl_triC_destroy(dev_trikC3000);
    return 0;
}

void *parserThread(void *arg) {
    /* Finite state machine array */
    state_dcode ( *fsm_dcode[dcode_numstates] )(dcode_cluster *cluster_in) =
            {[state_dcode_init] = state_dcodeFsm_init,
             [state_dcode_scan] = state_dcodeFsm_scan,
             [state_dcode_blkStart] = state_dcodeFsm_blkStart,
             [state_dcode_blkEnd] = state_dcodeFsm_blkEnd,
             [state_dcode_config] = state_dcodeFsm_config,
             [state_dcode_step] = state_dcodeFsm_step,
             [state_dcode_run] = state_dcodeFsm_run,
            [state_dcode_abort] = state_dcodeFsm_abort};
    state_dcode next_state = state_dcode_init;

    dcode_cluster *thread_cluster = state_dcodeFsm_setup( "test.dcode", &worker_queue);

    while (next_state != state_dcode_exit) {
        next_state = fsm_dcode[next_state](thread_cluster);
    }

    fclose(thread_cluster->file.file_pointer);
    return 0;
}