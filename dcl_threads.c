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
    void ( *fsm_triC[numStates] )() = {state_triC_idle, state_triC_init};

    dcl_serialDevice dev_trikC3000 = {
            .instrument_name = "TriKont Syringe",
            .dev_name = "/dev/tty.usbserial-0001",
            .dev_status = &pumpA_status,
    };

    printf("Connecting to %s\n", dev_trikC3000.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev_trikC3000) ) {
        printf("Connection to %s successful\n", dev_trikC3000.instrument_name);
    } else {
        return (void *)EXIT_FAILURE;
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
    dcl_triC_setValve(&dev_trikC3000, 4);
    sleep(2);
    dcl_triC_getStatus(&dev_trikC3000);
    dcl_triC_setPlunger(&dev_trikC3000, 0);
    sleep(4);

    dcl_serial_close(&dev_trikC3000);
    return 0;
}