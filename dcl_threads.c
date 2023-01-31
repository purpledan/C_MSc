//
// Created by Daniel on 2023/01/23.
//

#include "dcl_threads.h"

extern pthread_mutex_t pumpA_mutex;
extern pthread_cond_t pumpA_alert;
extern dcl_triC_status pumpA_status;
extern dcl_queue_type worker_queue;

void *pumpThread(void *arg) {
    /* Finite state machine array */
    state_triC ( *fsm_triC[numStates] )(state_triC_cluster_type *cluster_in) = {[state_idle] = state_triC_idle,
                                                                                [state_init] = state_triC_init,
                                                                                [state_getMsg] = state_triC_getMsg,
                                                                                [state_action] = state_triC_action,
                                                                                [state_transient] = state_triC_transient};
    state_triC next_state = state_init;

    dcl_serialDevice dev_trikC3000 = {
            .instrument_name = "TriKont Syringe",
            .dev_name = "/dev/ttyUSB0",
            .dev_status = &pumpA_status,
    };
    state_triC_cluster_type thread_cluster = {
            .device_in = & dev_trikC3000,
            .queue = &worker_queue
    };

    printf("Connecting to %s\n", dev_trikC3000.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev_trikC3000) ) {
        printf("Connection to %s successful\n", dev_trikC3000.instrument_name);
    } else {
        return (void *)EXIT_FAILURE;
    }

    while (next_state != state_exit) {
        next_state = fsm_triC[next_state](&thread_cluster);
    }
    /*
    pthread_mutex_lock(&pumpA_mutex);
    pthread_mutex_lock(&worker_queue.mutex);
    */

    printf("Valve: %d\n", ((dcl_triC_status *)dev_trikC3000.dev_status)->valve );
    printf("Plunger: %d\n", ((dcl_triC_status *)dev_trikC3000.dev_status)->plunger );
    printf("TopV: %d\n", ((dcl_triC_status *)dev_trikC3000.dev_status)->topV );
    printf("Init?: %b\n", ((dcl_triC_status *)dev_trikC3000.dev_status)->initialised );
    dcl_serial_close(&dev_trikC3000);
    return 0;
}