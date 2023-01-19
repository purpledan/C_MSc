
#include "dcl_triKont.h"

int main() {
    dcl_trik_cond stat_trikC3000 = {
            .address = 0,
    };
    dcl_serialDevice dev_trikC3000 = {
            .instrument_name = "TriKont Syringe",
            .dev_name = "/dev/tty.usbserial-0001",
            .dev_status = &stat_trikC3000,
    };
    dcl_string_box box_trikC3000 = {
            .box_name = "TriKont Syringe",
            .maxRetCnt = 8,
    };

    printf("Connecting to %s\n", dev_trikC3000.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev_trikC3000) ) {
        printf("Connection to %s successful\n", dev_trikC3000.instrument_name);
    } else {
        return EXIT_FAILURE;
    }
    /* Open storage container for the instrument, also critical */
    if ( dcl_string_box_open(&box_trikC3000) < 0) {
        return EXIT_FAILURE;
    }

    dcl_trik_cmd(&dev_trikC3000, init, cw, 0, &box_trikC3000);
    sleep(5);
    dcl_trik_cmd(&dev_trikC3000, status, query, 0, &box_trikC3000);
    dcl_string_box_strsep(&box_trikC3000, ',', 3);
    dcl_string_box_print(&box_trikC3000);

    dcl_string_box_close(&box_trikC3000);
    dcl_serial_close(&dev_trikC3000);
    return 0;
}
