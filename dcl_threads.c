//
// Created by Daniel on 2023/01/23.
//

#include "dcl_threads.h"

extern pthread_mutex_t pumpA_mutex;
extern pthread_cond_t pumpA_alert;
extern dcl_queue_type worker_queue;

void *pumpThread(void *arg) {
    /* Finite state machine array */
    state_triC ( *fsm_triC[numStates] )(triC_fsm_cluster *cluster_in) = {[state_idle] = state_triC_idle,
                                                                                [state_init] = state_triC_init,
                                                                                [state_getMsg] = state_triC_getMsg,
                                                                                [state_action] = state_triC_action,
                                                                                [state_transient] = state_triC_transient};
    state_triC next_state = state_init;

    dcl_serialDevice *dev_trikC3000 = dcl_triC_setup("TriKont Syringe",
                                                    "/dev/ttyUSB0",
                                                    0);
    if (!dev_trikC3000) {
        abort();
    }

    triC_fsm_cluster *thread_cluster = state_triC_fsmSetup(&worker_queue,
                                                          dev_trikC3000);


    while (next_state != state_exit) {
        next_state = fsm_triC[next_state](thread_cluster);
    }
    /*
    pthread_mutex_lock(&pumpA_mutex);
    pthread_mutex_lock(&worker_queue.mutex);
    */

    printf("Valve: %d\n", thread_cluster->status_in->valve );
    printf("Plunger: %d\n", thread_cluster->status_in->plunger );
    printf("TopV: %d\n", thread_cluster->status_in->topV );
    printf("Init?: %b\n", thread_cluster->status_in->initialised );
    dcl_serial_close(dev_trikC3000);
    return 0;
}