/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 2023/08/31. */

#include "dcl_heat.h"

dcl_serialDevice_heat *dcl_heat_setup(char name[], char serial_addr[]) {
    static dcl_serialDevice dev;
    static dcl_serialDevice_heat internal;
    static dcl_heat_status internal_status;

    internal.dev = &dev;
    internal.dev_select = 0;
    internal.dev_status = &internal_status;

    strncpy(dev.instrument_name, name, DEV_NAME_LEN - 1);
    strncpy(dev.dev_name, serial_addr, INSTRUMENT_NAME_LEN - 1);

    printf("Connecting to %s\n", dev.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev) ) {
        printf("File descriptor to %s ready\n", dev.instrument_name);
    } else {
        return NULL;
    }

    return &internal;
}

void dcl_heat_destroy(dcl_serialDevice_heat *device_in) {
    dcl_serial_close(device_in->dev);
}

void dcl_heat_init(dcl_serialDevice_heat *device_in) {
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, "PA_NEW");
    dcl_heat_read(device_in, read_buf);
}

void dcl_heat_getStatus(dcl_serialDevice_heat *device_in) {
    dcl_heat_getActTemp(device_in, 1);
    dcl_heat_getActSpeed(device_in);
    dcl_heat_getSetTemp(device_in);
    dcl_heat_getSetSpeed(device_in);

    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, "STATUS");
    dcl_heat_read(device_in, read_buf);
    dcl_heat_flipybit(read_buf);
    /*TODO: PARSE STATUS */
}

void dcl_heat_getActTemp(dcl_serialDevice_heat *device_in, bool sensor) {
    char read_buf[HEAT_READ_BUF] = "";
    if (sensor) {
        dcl_heat_write(device_in, "IN_PV_1");
    } else {
        dcl_heat_write(device_in, "IN_PV_3");
    }
    dcl_heat_read(device_in, read_buf);
    dcl_heat_flipybit(read_buf);
    char *valp = dcl_sstr_retWSpace_p(read_buf, HEAT_READ_BUF);
    if (valp) {
        device_in->dev_status->actual_temp = strtod(valp, NULL);
    }
}

void dcl_heat_getActSpeed(dcl_serialDevice_heat *device_in) {
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, "IN_PV_5");
    dcl_heat_read(device_in, read_buf);
    dcl_heat_flipybit(read_buf);
    char *valp = dcl_sstr_retWSpace_p(read_buf, HEAT_READ_BUF);
    if (valp) {
        device_in->dev_status->actual_speed = (int) strtol(valp, NULL, 10);
    }
}

void dcl_heat_getTMode(dcl_serialDevice_heat *device_in) {
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, "IN_MODE_4");
    dcl_heat_read(device_in, read_buf);
    dcl_heat_flipybit(read_buf);
    char *valp = dcl_sstr_retWSpace_p(read_buf, HEAT_READ_BUF);
    if (valp) {
        device_in->dev_status->temp_mode= (bool) strtol(valp, NULL, 10);
    }
}

void dcl_heat_getSetTemp(dcl_serialDevice_heat *device_in) {
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, "IN_SP_1");
    dcl_heat_read(device_in, read_buf);
    dcl_heat_flipybit(read_buf);
    char *valp = dcl_sstr_retWSpace_p(read_buf, HEAT_READ_BUF);
    if (valp) {
        device_in->dev_status->set_temp = strtod(valp, NULL);
    }
}
void dcl_heat_getSetSpeed(dcl_serialDevice_heat *device_in) {
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, "IN_SP_3");
    dcl_heat_read(device_in, read_buf);
    dcl_heat_flipybit(read_buf);
    char *valp = dcl_sstr_retWSpace_p(read_buf, HEAT_READ_BUF);
    if (valp) {
        device_in->dev_status->set_speed = (int) strtol(valp, NULL, 10);
    }
}

void dcl_heat_setTemp(dcl_serialDevice_heat *device_in, double set_temp) {
    device_in->dev_status->set_temp = set_temp;
    char cmd[HEAT_PRE_BUF] = "";
    /* Prevent bufferoverflows 101, maximum 5 chars for temp */
    if (set_temp > 300.0) {
        set_temp = 300.0;       /* Clamp to maximum temperature of heater*/
    } else if (set_temp < 0.0) {
        set_temp = 0.0;         /* Clamp to positive C scale */
    }
    sprintf(cmd, "OUT_SP_1 %.1f", set_temp);
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, cmd);
    dcl_heat_read(device_in, read_buf);
}

void dcl_heat_setSpeed(dcl_serialDevice_heat *device_in, int set_speed) {
    device_in->dev_status->set_temp = set_speed;
    char cmd[HEAT_PRE_BUF] = "";
    /* Prevent bufferoverflows 101, maximum 4 chars for speed */
    if (set_speed > 1400) {
        set_speed = 1400;       /* Clamp to maximum speed of stirrer*/
    } else if (set_speed < 0) {
        set_speed = 0;         /* Clamp to positive RPM scale */
    }
    sprintf(cmd, "OUT_SP_3 %d", set_speed);
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, cmd);
    dcl_heat_read(device_in, read_buf);
}

void dcl_heat_setTMode(dcl_serialDevice_heat *device_in, bool temp_mode) {
    device_in->dev_status->temp_mode = temp_mode;
    char cmd[HEAT_PRE_BUF] = "";
    sprintf(cmd, "OUT_MODE_4 %c", (temp_mode) ? '1' : '0');
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, cmd);
    dcl_heat_read(device_in, read_buf);
}

void dcl_heat_switchHeater(dcl_serialDevice_heat *device_in, bool heating) {
    device_in->dev_status->heating = heating;
    char cmd[HEAT_PRE_BUF] = "";
    sprintf(cmd, "%s_1", (heating) ? "START" : "STOP");
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, cmd);
    dcl_heat_read(device_in, read_buf);
}
void dcl_heat_switchStirring(dcl_serialDevice_heat *device_in, bool stirring) {
    device_in->dev_status->heating = stirring;
    char cmd[HEAT_PRE_BUF] = "";
    sprintf(cmd, "%s_2", (stirring) ? "START" : "STOP");
    char read_buf[HEAT_READ_BUF] = "";
    dcl_heat_write(device_in, cmd);
    dcl_heat_read(device_in, read_buf);
}

int dcl_heat_write(dcl_serialDevice_heat *device_in, char *cmd) {
    /* Commands are just sent raw towards the serial port, just needs a \r\n */
    char cmd_buffer[HEAT_CMD_BUF] = "";
    strcat(cmd_buffer, cmd);
    strcat(cmd_buffer, HEAT_TERM);

    dcl_serial_write(device_in->dev, cmd_buffer);

    return 0;
}

int dcl_heat_read(dcl_serialDevice_heat *device_in, char *read_buf) {
    ssize_t read_len = 0;
    read_len = dcl_serial_read(device_in->dev, read_buf, HEAT_READ_BUF - 1);
    if (read_len < 0) {
        return -1;
    }
    return read_len;
}

int dcl_heat_flipybit(char *read_buf) {
    /* The stupid hotplate returns chars with the most significant bit set at random times, switch them off */
    if (!read_buf) {
        return -1;
    }
    for (int i = 0; i < HEAT_READ_BUF - 1; i++) {
        read_buf[i] &= (char)0x7F; /* AND with NOT0x80 */
    }

    return 0;
}