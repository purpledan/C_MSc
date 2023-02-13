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
dcl_triC_status pump_status;
pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {

    // Init MSG queue
    dcl_queue_init(&worker_queue);
    // Test MSG
    dcl_strmsg_type buffer = {
            .terminate = 1,
            .argstr = "PUL,1,1500\0"
    };
    dcl_thr_sendMsg(&worker_queue, &buffer);

    int status;
    /*
    pthread_t worker_ID, parser_ID;
    status = pthread_create(&worker_ID, NULL, pumpThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }
    */
    pthread_t parser_ID;
    status = pthread_create(&parser_ID, NULL, parserThread, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }

    status = pthread_join(parser_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }
    /*
    status = pthread_join(worker_ID, NULL);
    if (status) {
        printf("Thread Fuckup, %d\n", status);
    }
     */
    return 0;
}
