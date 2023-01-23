//
// Created by Daniel on 2023/01/23.
//

#include "dcl_threads.h"
extern pthread_mutex_t pumpA_mutex;
extern pthread_cond_t pumpA_alert;
extern dcl_triC_status pumpA_status;

int pumpThread(void) {

    dcl_serialDevice dev_trikC3000 = {
            .instrument_name = "TriKont Syringe",
            .dev_name = "/dev/tty.usbserial-0001",
            .dev_status = &pumpA_status,
    };

    printf("Connecting to %s\n", dev_trikC3000.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev_trikC3000) ) {
        printf("Connection to %s successful\n", dev_trikC3000.instrument_name);
    } else {
        return EXIT_FAILURE;
    }




    dcl_serial_close(&dev_trikC3000);
    return 0;
}