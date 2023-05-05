/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2022/11/23 */

#include "dcl_triKont.h"

dcl_serialDevice *dcl_triC_monoSetup(char name[], char serial_addr[], int addr_switch) {
    static dcl_serialDevice dev;
    static dcl_triC_status internal;
    internal.address = addr_switch;
    dev.dev_status = &internal;

    strncpy(dev.instrument_name, name, DEV_NAME_LEN - 1);
    strncpy(dev.dev_name, serial_addr, INSTRUMENT_NAME_LEN - 1);

    printf("Connecting to %s\n", dev.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev) ) {
        printf("Connection to %s successful\n", dev.instrument_name);
    } else {
        return NULL;
    }

    return &dev;

}

dcl_serialDevice_triC *dcl_triC_multiSetup(char name[], char serial_addr[], int const addr_array[DCL_TRIC_PUMPNO]) {
    static dcl_serialDevice dev;
    static dcl_serialDevice_triC internal;
    static dcl_triC_status internal_status[DCL_TRIC_PUMPNO];

    internal.dev = &dev;
    internal.dev_select = 0;
    internal.dev_status_array = internal_status;

    for (int i = 0; i < DCL_TRIC_PUMPNO; i++) {
        internal.dev_status_array[i].address = addr_array[i];
    }

    strncpy(dev.instrument_name, name, DEV_NAME_LEN - 1);
    strncpy(dev.dev_name, serial_addr, INSTRUMENT_NAME_LEN - 1);

    printf("Connecting to %s\n", dev.instrument_name);
    /* Open a fd to a device, device is critical so should abort if not opened */
    if ( !dcl_serial_setup(&dev) ) {
        printf("Connection to %s successful\n", dev.instrument_name);
    } else {
        return NULL;
    }

    return &internal;
}

void dcl_triC_destroy(dcl_serialDevice_triC *device_in) {
    dcl_serial_close(device_in->dev);
}

void dcl_triC_init(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, "ZR");
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_setValve(dcl_serialDevice_triC *device_in, int valveNo) {
    char cmd[TRIC_PRE_BUF] = "";

    if (valveNo >= device_in->dev_status_array[device_in->dev_select].valve) {
        sprintf(cmd, "I%dR", valveNo);
    } else {
        sprintf(cmd, "O%dR", valveNo);
    }
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_setPlunger(dcl_serialDevice_triC *device_in, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "A%dR", value);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_aspirate(dcl_serialDevice_triC *device_in, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    sprintf(cmd, "P%dR", value);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_dispense(dcl_serialDevice_triC *device_in, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    sprintf(cmd, "D%dR", value);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_aspirateAtomic(dcl_serialDevice_triC *device_in, int valveNo, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    if (valveNo >= device_in->dev_status_array[device_in->dev_select].valve) {
        sprintf(cmd, "I%dP%dR", valveNo, value);
    } else {
        sprintf(cmd, "O%dP%dR", valveNo, value);
    }
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_dispenseAtomic(dcl_serialDevice_triC *device_in, int valveNo, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    if (valveNo >= device_in->dev_status_array[device_in->dev_select].valve) {
        sprintf(cmd, "I%dD%dR", valveNo, value);
    } else {
        sprintf(cmd, "O%dD%dR", valveNo, value);
    }
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_setAtomic(dcl_serialDevice_triC *device_in, int valveNo, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    if (valveNo >= device_in->dev_status_array[device_in->dev_select].valve) {
        sprintf(cmd, "I%dA%dR", valveNo, value);
    } else {
        sprintf(cmd, "O%dA%dR", valveNo, value);
    }
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_getStatus(dcl_serialDevice_triC *device_in) {
    size_t i = device_in->dev_select;
    device_in->dev_status_array[i].valve = dcl_triC_getValve(device_in);
    device_in->dev_status_array[i].plunger = dcl_triC_getPlunger(device_in);
    device_in->dev_status_array[i].statusByte = dcl_triC_getSByte(device_in);
}

void dcl_triC_getSetup(dcl_serialDevice_triC *device_in) {
    size_t i = device_in->dev_select;
    device_in->dev_status_array[i].valve = dcl_triC_getValve(device_in);
    device_in->dev_status_array[i].plunger = dcl_triC_getPlunger(device_in);
    device_in->dev_status_array[i].statusByte = dcl_triC_getSByte(device_in);
    device_in->dev_status_array[i].sV = dcl_triC_getsV(device_in);
    device_in->dev_status_array[i].topV = dcl_triC_getTopV(device_in);
    device_in->dev_status_array[i].slope = dcl_triC_getSlope(device_in);
    device_in->dev_status_array[i].initialised = dcl_triC_getInit(device_in);
}

int dcl_triC_getValve(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "?6");
    dcl_triC_read(device_in, read_buf);
    dcl_triC_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}
int dcl_triC_getPlunger(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "RZ");
    dcl_triC_read(device_in, read_buf);
    dcl_triC_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}
char dcl_triC_getSByte(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "Q");
    dcl_triC_read(device_in, read_buf);
    char status_byte = dcl_triC_parse(read_buf, &data);

    return status_byte;
}

int dcl_triC_getsV(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "?1");
    dcl_triC_read(device_in, read_buf);
    dcl_triC_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}

int dcl_triC_getTopV(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "?2");
    dcl_triC_read(device_in, read_buf);
    dcl_triC_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}
int dcl_triC_getSlope(dcl_serialDevice_triC *device_in){
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "?7");
    dcl_triC_read(device_in, read_buf);
    dcl_triC_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}
bool dcl_triC_getInit(dcl_serialDevice_triC *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    dcl_triC_write(device_in, "?19");
    dcl_triC_read(device_in, read_buf);
    dcl_triC_parse(read_buf, &data);

    if (data[0] == '0') {
        return false;
    } else {
        return true;
    }
}
void dcl_triC_setsV(dcl_serialDevice_triC *device_in, int startV) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "v%d", startV);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setSpeed(dcl_serialDevice_triC *device_in, int topV) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "S%dR", topV);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setV(dcl_serialDevice_triC *device_in, int topV) {
    char cmd[TRIC_PRE_BUF] = "";
    int accV;
    // See C3000 manual for hardcoded speed values
    if (topV >= 1400) {
        accV = 900;
    } else {
        accV = (int)lround( topV * 1.555 );
    }

    sprintf(cmd, "v%dV%dc%dR", accV, topV, accV);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setSlope(dcl_serialDevice_triC *device_in, int accSlope) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "L%d", accSlope);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setRes(dcl_serialDevice_triC *device_in, int resolution) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "N%d", resolution);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
    device_in->dev_status_array[device_in->dev_select].plungerResolution = resolution;
}

void dcl_triC_multiGetStatus(dcl_serialDevice_triC *device_in) {
    size_t temp = device_in->dev_select;
    for (size_t i = 0; i < DCL_TRIC_PUMPNO; i++) {
        device_in->dev_select = i;
        dcl_triC_getStatus(device_in);
    }
    device_in->dev_select = temp;
}

void dcl_triC_multiGetSetup(dcl_serialDevice_triC *device_in) {
    size_t temp = device_in->dev_select;
    for (size_t i = 0; i < DCL_TRIC_PUMPNO; i++) {
        device_in->dev_select = i;
        dcl_triC_getSetup(device_in);
    }
    device_in->dev_select = temp;
}

bool dcl_triC_isBusy(dcl_serialDevice_triC *device_in, size_t address) {
    size_t temp = device_in->dev_select;

    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    char status_byte = '\0';
    dcl_triC_write(device_in, "Q");
    dcl_triC_read(device_in, read_buf);
    status_byte = dcl_triC_parse(read_buf, &data);

    device_in->dev_select = temp;
    if (status_byte == '@') {
        return true;
    } else {
        return false;
    }
}

int dcl_triC_write(dcl_serialDevice_triC *device_in, char *cmd) {
    /* '/' is needed for cmd start, then pick out dev_status from the void pointer
     * in order to populate address label. */
    char cmd_buffer[TRIC_CMD_BUF] = "";
    sprintf(cmd_buffer, "/%d", (int) device_in->dev_select + 1); // switch setting on pump needs +1 to work
    strcat(cmd_buffer, cmd);
    strcat(cmd_buffer, TRIC_TERM);

    dcl_serial_write(device_in->dev, cmd_buffer);

    return 0;
}
int dcl_triC_read(dcl_serialDevice_triC *device_in, char *read_buf) {

    ssize_t read_len = 0;
    read_len = dcl_serial_read(device_in->dev, read_buf, TRIC_READ_BUF - 1);
    if (read_len < 0) {
        return -1;
    }
    return read_len;
}

char dcl_triC_parse(char *read_buf, char **ret_data) {
    //printf("Buf: %s\n", read_buf);
    char *indexer = NULL;
    char ret_byte = '\0';
    indexer = strrchr(read_buf, 0x03);
    if (!indexer) {
        return '\0';
    }
    *indexer = '\0';

    indexer = strchr(read_buf, '0');
    if (!indexer) {
        return '\0';
    }
    ret_byte = *(++indexer);
    *indexer = '\0';
    *ret_data = ++indexer;

    return ret_byte;
}