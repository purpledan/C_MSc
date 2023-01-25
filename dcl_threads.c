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
                                                                                [state_init] = state_triC_init};
    state_triC next_state = state_init;

    dcl_serialDevice dev_trikC3000 = {
            .instrument_name = "TriKont Syringe",
            .dev_name = "/dev/tty.usbserial-0001",
            .dev_status = &pumpA_status,
    };
    state_triC_cluster_type thread_cluster = {
            .device_in = & dev_trikC3000,
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
    //dcl_triC_init(&dev_trikC3000);

    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setValve(&dev_trikC3000, 1);
    sleep(2);
    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setPlunger(&dev_trikC3000, 1500);
    sleep(4);
    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setValve(&dev_trikC3000, 2);
    sleep(2);
    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setPlunger(&dev_trikC3000, 3000);
    sleep(5);
    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setValve(&dev_trikC3000, 6);
    sleep(2);
    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setPlunger(&dev_trikC3000, 0);
    sleep(4);

    dcl_serial_close(&dev_trikC3000);
    return 0;
}