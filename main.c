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
            .terminate = 0,
            .argstr = "PUL,1,1500\0"
    };
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "SET,0,16\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "PUL,2,1500\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "SET,0,9\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "PSH,6,3000\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);
    strcpy(buffer.argstr, "SET,0,11\0");
    dcl_thr_sendMsg(&worker_queue, &buffer);

    //Terminate
    buffer.terminate = 1;
    strcpy(buffer.argstr, "");
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
