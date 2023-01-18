//
// Created by Daniel on 2023/01/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TST_MsgQueue.h"

void * read_thread(void *arg) {
    Msg_data_type buffer;
    char queueStatus;
    size_t queueLen;

    while (1) {
        pthread_mutex_lock(&MsgQueue.mutex);
        queueLen = MsgQueue.length;
        TST_MsgRead(&MsgQueue, &buffer);
        queueStatus = MsgQueue.status;
        if (queueStatus == 'X') {
            return NULL;
        } else if (!queueLen) {
            printf("Queue empty, waiting...\n");
            pthread_cond_wait(&MsgQueue.alert, &MsgQueue.mutex);
            pthread_mutex_unlock(&MsgQueue.mutex);

        } else {
            pthread_mutex_unlock(&MsgQueue.mutex);
            printf("Read ID:%d Msg:%s\n", buffer.ID, buffer.message);
        }
    }
}

void TST_MsgQueue(DCL_Queue_type *MsgQ, Msg_data_type *buffer) {
    Msg_data_type *p;

    p = malloc(sizeof(Msg_data_type));
    if (!p) {
        perror("Malloc for new test data");
        abort();
    }

    p->ID = buffer->ID;
    stpncpy(p->message, buffer->message, MSG_LEN);

    DCL_Queue_pushBack(MsgQ, p);
}

int TST_MsgRead(DCL_Queue_type *MsgQ, Msg_data_type *buffer) {
    if (!MsgQ->length) {
        return 0;
    }
    Msg_data_type *p = DCL_Queue_pop(MsgQ);
    if (!p) {
        return -1;
    }

    int l = 0;
    buffer->ID = p->ID;
     l = strlcpy(buffer->message, p->message, MSG_LEN);
    free(p);

    return l;
}

void TST_GenMsg(Msg_data_type *buffer) {
    buffer->ID = rand()%100;

    for (int i = 0; i < MSG_LEN - 1; i++) {
        buffer->message[i] = (char)('A' + (rand()%('Z' - 'A')));
    }
    buffer->message[MSG_LEN - 1] = '\0';
}

int Msg_test_main() {
    DCL_Queue_init(&MsgQueue);
    Msg_data_type buffer;

    int status;
    pthread_t read_thread_id;

    status = pthread_create(&read_thread_id, NULL, read_thread, NULL);
    if (status) {
        fprintf(stderr, "Error: %d", status);
    }


    for (int i = 0; i < TEST_LEN; i++) {
        TST_GenMsg(&buffer);
        printf("Operation:%d ID:%d MSG:%s\n", i, buffer.ID, buffer.message);
        pthread_mutex_lock(&MsgQueue.mutex);
        TST_MsgQueue(&MsgQueue, &buffer);
        pthread_mutex_unlock(&MsgQueue.mutex);
        pthread_cond_signal(&MsgQueue.alert);
        sleep(1);
    }
    pthread_mutex_lock(&MsgQueue.mutex);
    MsgQueue.status = 'X';
    pthread_cond_signal(&MsgQueue.alert);
    pthread_mutex_unlock(&MsgQueue.mutex);
    pthread_join(read_thread_id, NULL);
    return 0;
}
