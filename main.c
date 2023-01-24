#include <pthread.h>
#include "dcl_triKont.h"
#include "dcl_threads.h"

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
