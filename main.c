#include <pthread.h>
#include "dcl_triKont.h"
#include "dcl_threads.h"
#include "dcl_msg_triKont.h"

/* Global Variables */
dcl_queue_type worker_queue = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .alert = PTHREAD_COND_INITIALIZER
};

pthread_mutex_t pumpA_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pumpA_alert = PTHREAD_COND_INITIALIZER;

dcl_triC_status pumpA_status = {
        .address = 0,
        .statusByte = '\0'
};

int main() {

    // Init MSG queue
    dcl_queue_init(&worker_queue);
    // Test MSG
    msg_type buffer = {
            .terminate = 0,
            .arg = "PUL,1,3000\0"
    };
    triC_queueMsg(&worker_queue, &buffer);

    int status;
    pthread_t worker_ID;
    status = pthread_create(&worker_ID, NULL, pumpThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_join(worker_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }
    return 0;
}
