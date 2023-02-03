//
// Created by Daniel on 2023/01/23.
//

#include "dcl_threads.h"
extern dcl_triC_status pump_status;
extern pthread_mutex_t status_mutex;
extern dcl_queue_type worker_queue;

void *pumpThread(void *arg) {
    /* Finite state machine array */
    state_triC ( *fsm_triC[numStates] )(triC_fsm_cluster *cluster_in) = {[state_idle] = state_triC_idle,
                                                                                [state_init] = state_triC_init,
                                                                                [state_getMsg] = state_triC_getMsg,
                                                                                [state_action] = state_triC_action,
                                                                                [state_transient] = state_triC_transient,
                                                                                [state_critical] = state_triC_critical,
                                                                                [state_terminate] = state_triC_terminate};
    state_triC next_state = state_init;

    dcl_serialDevice *dev_trikC3000 = dcl_triC_setup("TriKont Syringe",
                                                    "/dev/ttyUSB0",
                                                    0);
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

    dcl_serial_close(dev_trikC3000);
    return 0;
}