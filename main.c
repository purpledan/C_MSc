#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "DCL_MsgQueue.h"
#include "TST_MsgQueue.h"

#define TEST_LEN 10

DCL_Queue_type MsgQueue = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .alert = PTHREAD_COND_INITIALIZER
};

void * read_thread(void *arg) {
    Msg_data_type buffer;
    int status;
    sleep(1);
    status = pthread_mutex_lock(&MsgQueue.mutex);
    if (status != 0) {
        fprintf(stderr, "Error: %d", status);
    }
    TST_MsgRead(&MsgQueue, &buffer);
    status = pthread_mutex_unlock(&MsgQueue.mutex);
    if (status != 0) {
        fprintf(stderr, "Error: %d", status);
    }
    printf("Read ID:%d Msg:%s", buffer.ID, buffer.message);
    return NULL;
}
int main() {

    DCL_Queue_init(&MsgQueue);

    Msg_data_type buffer;
    for (int i = 0; i < TEST_LEN; i++) {
        TST_GenMsg(&buffer);
        printf("Operation:%d ID:%d MSG:%s\n", i, buffer.ID, buffer.message);
        TST_MsgQueue(&MsgQueue, &buffer);
    }

    for (int i = 0; i < TEST_LEN; i++) {
        TST_MsgRead(&MsgQueue, &buffer);
        printf("Operation:%d ID:%d MSG:%s\n", i, buffer.ID, buffer.message);
    }

    return 0;
}
