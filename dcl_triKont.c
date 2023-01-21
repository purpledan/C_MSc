//
// Created by Daniel on 2022/11/23.
//

#include "dcl_triKont.h"

void dcl_trik_init(dcl_serialDevice *device_in) {
    char read_buf[TRIK_READ_BUF] = "";
    dcl_trik_write(device_in, "ZR");
    dcl_trik_read(device_in, read_buf);
}

void dcl_trik_setValve(dcl_serialDevice *device_in, int valveNo) {
    char cmd[8] = "";

    if (valveNo >= TRIK_STATUS->valve) {
        sprintf(cmd, "I%dR", valveNo);
    } else {
        sprintf(cmd, "O%dR", valveNo);
    }
    char read_buf[TRIK_READ_BUF] = "";
    dcl_trik_write(device_in, cmd);
    dcl_trik_read(device_in, read_buf);
}

void dcl_trik_setPlunger(dcl_serialDevice *device_in, int value) {
    char cmd[8] = "";
    sprintf(cmd, "A%dR", value);
    char read_buf[TRIK_READ_BUF] = "";
    dcl_trik_write(device_in, cmd);
    dcl_trik_read(device_in, read_buf);
}

void dcl_trik_aspirate(dcl_serialDevice *device_in, int value) {
    char cmd[8] = "";
    // TODO: Implement limit based on resolution
    sprintf(cmd, "P%dR", value);
    char read_buf[TRIK_READ_BUF] = "";
    dcl_trik_write(device_in, cmd);
    dcl_trik_read(device_in, read_buf);
}

void dcl_trik_dispense(dcl_serialDevice *device_in, int value) {
    char cmd[8] = "";
    // TODO: Implement limit based on resolution
    sprintf(cmd, "D%dR", value);
    char read_buf[TRIK_READ_BUF] = "";
    dcl_trik_write(device_in, cmd);
    dcl_trik_read(device_in, read_buf);
}

void dcl_trik_getStatus(dcl_serialDevice *device_in) {
    TRIK_STATUS->valve = dcl_trik_getValve(device_in);
    TRIK_STATUS->plunger = dcl_trik_getPlunger(device_in);
    TRIK_STATUS->statusByte = dcl_trik_getSByte(device_in);
}

int dcl_trik_getValve(dcl_serialDevice *device_in) {
    char read_buf[TRIK_READ_BUF] = "";
    char *data = NULL;
    dcl_trik_write(device_in, "?6");
    dcl_trik_read(device_in, read_buf);
    dcl_trik_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}
int dcl_trik_getPlunger(dcl_serialDevice *device_in) {
    char read_buf[TRIK_READ_BUF] = "";
    char *data = NULL;
    dcl_trik_write(device_in, "RZ");
    dcl_trik_read(device_in, read_buf);
    dcl_trik_parse(read_buf, &data);
    if (!data) {
        return -1;
    }

    return atoi(data);
}
char dcl_trik_getSByte(dcl_serialDevice *device_in) {
    char read_buf[TRIK_READ_BUF] = "";
    char *data = NULL;
    char status_byte = '\0';
    dcl_trik_write(device_in, "Q");
    dcl_trik_read(device_in, read_buf);
    status_byte = dcl_trik_parse(read_buf, &data);

    return status_byte;
}

int dcl_trik_write(dcl_serialDevice *device_in, char *cmd) {
    /* '/' is needed for cmd start, then pick out dev_status from the void pointer
     * in order to populate address label. */
    char cmd_buffer[TRIK_CMD_BUF] = "";
    sprintf(cmd_buffer, "/%d", ((dcl_trik_status *)device_in->dev_status)->address + 1); // switch setting on pump needs +1 to work
    strcat(cmd_buffer, cmd);
    strcat(cmd_buffer, TRIK_TERM);

    dcl_serial_write(device_in, cmd_buffer);

    return 0;
}
int dcl_trik_read(dcl_serialDevice *device_in, char *read_buf) {

    ssize_t read_len = 0;
    read_len = dcl_serial_read(device_in, read_buf, TRIK_READ_BUF - 1);
    if (read_len < 0) {
        return -1;
    }
    return read_len;
}

char dcl_trik_parse(char *read_buf, char **ret_data) {
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