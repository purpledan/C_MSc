/*X********************************************************************************************************************
oooooooooo.     .oooooo.   ooooo
`888'   `Y8b   d8P'  `Y8b  `888'
 888      888 888           888
 888      888 888           888
 888      888 888           888
 888     d88' `88b    ooo   888       o
o888bood8P'    `Y8bood8P'  o888ooooood8
                                                      .o.                 .o8        o8o      .
                                                     .888.               "888        `"'    .o8
                                                    .8"888.     oooo d8b  888oooo.  oooo  .o888oo  .ooooo.  oooo d8b
                                                   .8' `888.    `888""8P  d88' `88b `888    888   d88' `88b `888""8P
                                                  .88ooo8888.    888      888   888  888    888   888ooo888  888
                                                 .8'     `888.   888      888   888  888    888 . 888    .o  888
                                                o88o     o8888o d888b     `Y8bod8P' o888o   "888" `Y8bod8P' d888b
       __
 /    /  )_   '_ /
()(/ /(_/(//)/(-(
  /
**********************************************************************************************************************/
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
dcl_queue_type pump_queue = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .alert = PTHREAD_COND_INITIALIZER
};
dcl_queue_type arb_queue = {
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

    dcl_queue_init(&pump_queue);

    int status;

    pthread_t pumpTh_ID, parser_ID, arb_ID;

    status = pthread_create(&parser_ID, NULL, parserThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_create(&pumpTh_ID, NULL, pumpThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_create(&arb_ID, NULL, arbThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_join(parser_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_join(pumpTh_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_join(arb_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }
    return 0;
}
