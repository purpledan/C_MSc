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

pthread_mutex_t pumpA_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pumpA_alert = PTHREAD_COND_INITIALIZER;


int main() {

    // Init MSG queue
    dcl_queue_init(&worker_queue);
    // Test MSG
    dcl_strmsg_type buffer = {
            .terminate = 0,
            .argstr = "PUL,1,1500\0"
    };
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "PUL,2,1500\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "PSH,6,3000\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);

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
