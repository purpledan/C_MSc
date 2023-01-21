//
// Created by Daniel on 2022/11/23.
//

#ifndef LIB_DCL_TRIKONT_H
#define LIB_DCL_TRIKONT_H

#include <stdbool.h>

#include "dcl_string.h"
#include "dcl_serial.h"

#define TRIK_CMD_BUF 32
#define TRIK_READ_BUF 32
#define TRIK_TERM "\r\n"
#define TRIK_6D 6
#define TRIK_STATUS ((dcl_trik_status *)device_in->dev_status)

typedef struct dcl_trik_status{
    int address;                    // Switch setting on pump
    int valve;                      // Valve position
    int plunger;                    // Plunger position
    int plungerResolution;          // see C3000 manual pg62
    int slope;                      // acc slope manual pg66
    int sV;                         // start velocity manual pg67
    int topV;                       // top velocity manual pg 67
    char statusByte;                // Status Byte
} dcl_trik_status;

void dcl_trik_init(dcl_serialDevice *device_in);
void dcl_trik_setValve(dcl_serialDevice *device_in, int valveNo);
void dcl_trik_setPlunger(dcl_serialDevice *device_in, int value);
void dcl_trik_aspirate(dcl_serialDevice *device_in, int value);
void dcl_trik_dispense(dcl_serialDevice *device_in, int value);
void dcl_trik_getStatus(dcl_serialDevice *device_in);
int dcl_trik_getValve(dcl_serialDevice *device_in);
int dcl_trik_getPlunger(dcl_serialDevice *device_in);
char dcl_trik_getSByte(dcl_serialDevice *device_in);

char dcl_trik_parse(char *read_buf, char **ret_data);

int dcl_trik_write(dcl_serialDevice *device_in, char *cmd);
int dcl_trik_read(dcl_serialDevice *device_in, char *read_buf);
#endif //LIB_DCL_TRIKONT_H
