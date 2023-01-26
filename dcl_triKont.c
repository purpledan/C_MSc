//
// Created by Daniel on 2022/11/23.
//

#include "dcl_triKont.h"

void dcl_triC_init(dcl_serialDevice *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, "ZR");
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_setValve(dcl_serialDevice *device_in, int valveNo) {
    char cmd[TRIC_PRE_BUF] = "";

    if (valveNo >= TRIC_STATUS->valve) {
        sprintf(cmd, "I%dR", valveNo);
    } else {
        sprintf(cmd, "O%dR", valveNo);
    }
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_setPlunger(dcl_serialDevice *device_in, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "A%dR", value);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_aspirate(dcl_serialDevice *device_in, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    sprintf(cmd, "P%dR", value);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_dispense(dcl_serialDevice *device_in, int value) {
    char cmd[TRIC_PRE_BUF] = "";
    // TODO: Implement limit based on resolution
    sprintf(cmd, "D%dR", value);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}

void dcl_triC_getStatus(dcl_serialDevice *device_in) {
    TRIC_STATUS->valve = dcl_triC_getValve(device_in);
    TRIC_STATUS->plunger = dcl_triC_getPlunger(device_in);
    TRIC_STATUS->statusByte = dcl_triC_getSByte(device_in);
}

void dcl_triC_getSetup(dcl_serialDevice *device_in) {
    TRIC_STATUS->valve = dcl_triC_getValve(device_in);
    TRIC_STATUS->plunger = dcl_triC_getPlunger(device_in);
    TRIC_STATUS->statusByte = dcl_triC_getSByte(device_in);
    TRIC_STATUS->sV = dcl_triC_getsV(device_in);
    TRIC_STATUS->topV = dcl_triC_getTopV(device_in);
    TRIC_STATUS->slope = dcl_triC_getSlope(device_in);
    TRIC_STATUS->initialised = dcl_triC_getInit(device_in);
}

int dcl_triC_getValve(dcl_serialDevice *device_in) {
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
int dcl_triC_getPlunger(dcl_serialDevice *device_in) {
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
char dcl_triC_getSByte(dcl_serialDevice *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    char status_byte = '\0';
    dcl_triC_write(device_in, "Q");
    dcl_triC_read(device_in, read_buf);
    status_byte = dcl_triC_parse(read_buf, &data);

    return status_byte;
}

int dcl_triC_getsV(dcl_serialDevice *device_in) {
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

int dcl_triC_getTopV(dcl_serialDevice *device_in) {
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
int dcl_triC_getSlope(dcl_serialDevice *device_in){
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
bool dcl_triC_getInit(dcl_serialDevice *device_in) {
    char read_buf[TRIC_READ_BUF] = "";
    char *data = NULL;
    char status_byte = '\0';
    dcl_triC_write(device_in, "?19");
    dcl_triC_read(device_in, read_buf);
    status_byte = dcl_triC_parse(read_buf, &data);

    if (status_byte == '0') {
        return false;
    } else {
        return true;
    }
}
void dcl_triC_setsV(dcl_serialDevice *device_in, int startV) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "v%d", startV);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setTopV(dcl_serialDevice *device_in, int topV) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "V%d", topV);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setSlope(dcl_serialDevice *device_in, int accSlope) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "L%d", accSlope);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
}
void dcl_triC_setRes(dcl_serialDevice *device_in, int resolution) {
    char cmd[TRIC_PRE_BUF] = "";
    sprintf(cmd, "N%d", resolution);
    char read_buf[TRIC_READ_BUF] = "";
    dcl_triC_write(device_in, cmd);
    dcl_triC_read(device_in, read_buf);
    TRIC_STATUS->plungerResolution = resolution;
}

int dcl_triC_write(dcl_serialDevice *device_in, char *cmd) {
    /* '/' is needed for cmd start, then pick out dev_status from the void pointer
     * in order to populate address label. */
    char cmd_buffer[TRIC_CMD_BUF] = "";
    sprintf(cmd_buffer, "/%d", ((dcl_triC_status *)device_in->dev_status)->address + 1); // switch setting on pump needs +1 to work
    strcat(cmd_buffer, cmd);
    strcat(cmd_buffer, TRIC_TERM);

    dcl_serial_write(device_in, cmd_buffer);

    return 0;
}
int dcl_triC_read(dcl_serialDevice *device_in, char *read_buf) {

    ssize_t read_len = 0;
    read_len = dcl_serial_read(device_in, read_buf, TRIC_READ_BUF - 1);
    if (read_len < 0) {
        return -1;
    }
    return read_len;
}

char dcl_triC_parse(char *read_buf, char **ret_data) {
    char *indexer = NULL;
    char ret_byte = '\0';
    printf("ReadBuf: %s", read_buf);
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