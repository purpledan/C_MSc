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

void *parserThread(void *arg) {
    /* Finite state machine array */
    state_dcode ( *fsm_dcode[dcode_numstates] )(dcode_cluster *cluster_in) =
            {[state_dcode_init] = state_dcodeFsm_init,
             [state_dcode_scan] = state_dcodeFsm_scan,
             [state_dcode_blkStart] = state_dcodeFsm_blkStart,
             [state_dcode_blkEnd] = state_dcodeFsm_blkEnd,
             [state_dcode_config] = state_dcodeFsm_config};
    state_dcode next_state = state_dcode_init;

    dcode_cluster *thread_cluster = state_dcodeFsm_setup( "test.dcode", NULL);

    while (next_state != state_dcode_exit) {
        next_state = fsm_dcode[next_state](thread_cluster);
    }
    printf("Printing read values:\n");
    for (int i = 0; i < 6; i++) {
        printf("Valve: %d; Name: %s; Speed: %d\n", i + 1,
               thread_cluster->config.valve_names[i],
               thread_cluster->config.valve_speeds[i]);
    }
    printf("Default speed: %d\n", thread_cluster->config.default_speed);

    fclose(thread_cluster->file.file_pointer);
    return 0;
}