
/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/

#include <pthread.h>
#include <string.h>
#include "dcl_triKont.h"
#include "dcl_threads.h"
#include "dcl_fsm.h"

/* Global Variables */
dcl_queue_type worker_queue = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .alert = PTHREAD_COND_INITIALIZER
};
dcl_triC_status global_pump_array[DCL_TRIC_PUMPNO];
pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    printf("\x1b[2J");
    printf("\x1b[H");
    printf("\x1b[1;33mWelcome to DCL C3000 SP Arbiter v0.1\n");
    printf("\x1b[22;34mThis is pre alpha software, things will go wrong\n");
    printf("\x1b[2mThe author can't be held responsible if attached appliances inflict damages to itself, property, and or people. ");
    printf("Further, in case of attached appliances gaining sentience, their actions can't be said to be endorsed by the author.\n\n");

    printf("\x1b[0;32mStarting Arbiter & DCODE threads:\n");

    dcl_queue_init(&worker_queue);

    int status;

    pthread_t worker_ID, parser_ID;

    status = pthread_create(&parser_ID, NULL, parserThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_create(&worker_ID, NULL, pumpThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }


    status = pthread_join(parser_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_join(worker_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }
    return 0;
}
