//
// Created by Daniel on 2022/11/23.
//

#ifndef LIB_DCL_TRIKONT_H
#define LIB_DCL_TRIKONT_H

#include <stdbool.h>

#include "dcl_string.h"
#include "dcl_serial.h"
#define TRIC_PRE_BUF 16
#define TRIC_CMD_BUF 32
#define TRIC_READ_BUF 32
#define TRIC_TERM "\r\n"
#define TRIC_STATUS ((dcl_triC_status *)device_in->dev_status)

typedef struct dcl_triC_status{
    int address;                    // Switch setting on pump
    int valve;                      // Valve position
    int plunger;                    // Plunger position
    int plungerResolution;          // see C3000 manual pg62
    int slope;                      // acc slope manual pg66
    int sV;                         // start velocity manual pg67
    int topV;                       // top velocity manual pg 67
    bool initialised;               // is pump initialised?
    char statusByte;                // Status Byte
} dcl_triC_status;

/* General setup function */
dcl_serialDevice *dcl_triC_setup(char name[], char serial_addr[], int addr_switch);
/* Device specific functions */
void dcl_triC_init(dcl_serialDevice *device_in);
void dcl_triC_setValve(dcl_serialDevice *device_in, int valveNo);
void dcl_triC_setPlunger(dcl_serialDevice *device_in, int value);
void dcl_triC_aspirate(dcl_serialDevice *device_in, int value);
void dcl_triC_dispense(dcl_serialDevice *device_in, int value);
void dcl_triC_getStatus(dcl_serialDevice *device_in);
void dcl_triC_getSetup(dcl_serialDevice *device_in);
int dcl_triC_getValve(dcl_serialDevice *device_in);
int dcl_triC_getPlunger(dcl_serialDevice *device_in);
char dcl_triC_getSByte(dcl_serialDevice *device_in);
int dcl_triC_getsV(dcl_serialDevice *device_in);
int dcl_triC_getTopV(dcl_serialDevice *device_in);
int dcl_triC_getSlope(dcl_serialDevice *device_in);
bool dcl_triC_getInit(dcl_serialDevice *device_in);
void dcl_triC_setsV(dcl_serialDevice *device_in, int startV);
void dcl_triC_setTopV(dcl_serialDevice *device_in, int topV);
void dcl_triC_setSlope(dcl_serialDevice *device_in, int accSlope);
void dcl_triC_setRes(dcl_serialDevice *device_in, int resolution);
void dcl_triC_setSpeed(dcl_serialDevice *device_in, int topV);
/* Helper functions */
char dcl_triC_parse(char *read_buf, char **ret_data);
/* Raw IO functions */
int dcl_triC_write(dcl_serialDevice *device_in, char *cmd);
int dcl_triC_read(dcl_serialDevice *device_in, char *read_buf);
#endif //LIB_DCL_TRIKONT_H
