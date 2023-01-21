
#include "dcl_triKont.h"

int main() {
    dcl_trik_status stat_trikC3000 = {
            .address = 0,
            .plunger = 0,
            .statusByte = '\0'
    };
    dcl_serialDevice dev_trikC3000 = {
            .instrument_name = "TriKont Syringe",
            .dev_name = "/dev/tty.usbserial-0001",
            .dev_status = &stat_trikC3000,
    };

    printf("Connecting to %s\n", dev_trikC3000.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev_trikC3000) ) {
        printf("Connection to %s successful\n", dev_trikC3000.instrument_name);
    } else {
        return EXIT_FAILURE;
    }
    dcl_trik_init(&dev_trikC3000);

    dcl_trik_getStatus(&dev_trikC3000);
    printf("Current status of pump: Valve = %d, Plunger = %d, Byte = %c\n", stat_trikC3000.valve, stat_trikC3000.plunger, stat_trikC3000.statusByte);
    dcl_trik_setValve(&dev_trikC3000, 4);
    sleep(1);

    dcl_trik_setPlunger(&dev_trikC3000, 0);
    sleep(5);

    dcl_trik_getStatus(&dev_trikC3000);
    printf("Current status of pump: Valve = %d, Plunger = %d, Byte = %c\n", stat_trikC3000.valve, stat_trikC3000.plunger, stat_trikC3000.statusByte);

    dcl_serial_close(&dev_trikC3000);
    return 0;
}
