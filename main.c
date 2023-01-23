
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
        .address = -1,
        .plunger = -1,
        .statusByte = '\0'
};

int main() {

    dcl_trik_init(&dev_trikC3000);

    dcl_trik_getStatus(&dev_trikC3000);
    printf("Current status of pump: Valve = %d, Plunger = %d, Byte = %c\n", stat_trikC3000.valve, stat_trikC3000.plunger, stat_trikC3000.statusByte);
    dcl_trik_setValve(&dev_trikC3000, 4);
    sleep(1);

    dcl_trik_setPlunger(&dev_trikC3000, 0);
    sleep(5);

    dcl_trik_getStatus(&dev_trikC3000);
    printf("Current status of pump: Valve = %d, Plunger = %d, Byte = %c\n", stat_trikC3000.valve, stat_trikC3000.plunger, stat_trikC3000.statusByte);


    return 0;
}
