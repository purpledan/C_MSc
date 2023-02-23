//
// Created by Daniel on 2023/01/16.
//

#ifndef C_MSC_TST_MSGQUEUE_H
#define C_MSC_TST_MSGQUEUE_H

#include "dcl_structs.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MSG_LEN 32
#define TEST_LEN 10

static DCL_Queue_type MsgQueue = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .alert = PTHREAD_COND_INITIALIZER,
        .status = 'R'
};

typedef struct Msg_data {
    int ID;
    char message[MSG_LEN];
} Msg_data_type;

void TST_MsgQueue(DCL_Queue_type *MsgQ, Msg_data_type *buffer);
int TST_MsgRead(DCL_Queue_type *MsgQ, Msg_data_type *buffer);

void TST_GenMsg(Msg_data_type *buffer);

void * read_thread(void *arg);
int Msg_test_main();
#endif //C_MSC_TST_MSGQUEUE_H
