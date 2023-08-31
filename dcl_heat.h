/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 2023/08/31. */

/*  Files: dcl_heat.h & dcl_heat.c
 *  Library to control Radleys Connect hotplates
 *
 *  Uses POSIX to open and communicate with a serial device
 *
 */

#ifndef C_MSC_DCL_HEAT_H
#define C_MSC_DCL_HEAT_H

#include <stdbool.h>
#include <math.h>

#include "dcl_string.h"
#include "dcl_serial.h"
#include "dcl_config.h"

#define HEAT_PRE_BUF 16
#define HEAT_CMD_BUF 32
#define HEAT_READ_BUF 32
#define HEAT_TERM "\r\n"

typedef struct dcl_heat_status{
    double actual_temp;             // Actual Temp in C
    double actual_speed;            // Actual speed of stirring in RPM
    double set_temp;                // Set Temp in C
    int set_speed;               // Set speed of stirring in RPM
    bool   temp_mode;               // Temperature control mode, 0: Precise-Mode 1: Fast-Mode
    bool   stirring;                // Stirring is on
    bool   heating;                 // Heating is on
    bool initialised;               // is pump initialised?
    char statusByte;                // Status Byte
} dcl_heat_status;

typedef struct dcl_serialDevice_heat {
    dcl_serialDevice *dev;
    size_t dev_select;
    dcl_heat_status *dev_status;
} dcl_serialDevice_heat;

/* General Setup Function */
dcl_serialDevice_heat *dcl_heat_setup(char name[], char serial_addr[]);
/* General Destroy Function */
void dcl_heat_destroy(dcl_serialDevice_heat *device_in);
/* Device Specific Functions */
void dcl_heat_init(dcl_serialDevice_heat *device_in);
void dcl_heat_getStatus(dcl_serialDevice_heat *device_in);
void dcl_heat_getActTemp(dcl_serialDevice_heat *device_in);
void dcl_heat_getActSpeed(dcl_serialDevice_heat *device_in);
void dcl_heat_getTMode(dcl_serialDevice_heat *device_in);
void dcl_heat_getSetTemp(dcl_serialDevice_heat *device_in);
void dcl_heat_getSetSpeed(dcl_serialDevice_heat *device_in);
void dcl_heat_setTemp(dcl_serialDevice_heat *device_in, double set_temp);
void dcl_heat_setSpeed(dcl_serialDevice_heat *device_in, int set_speed);
void dcl_heat_setTMode(dcl_serialDevice_heat *device_in, bool temp_mode);
void dcl_heat_switchHeater(dcl_serialDevice_heat *device_in, bool heating);
void dcl_heat_switchStirring(dcl_serialDevice_heat *device_in, bool stirring);
/* Raw IO Functions */
int dcl_heat_write(dcl_serialDevice_heat *device_in, char *cmd);
int dcl_heat_read(dcl_serialDevice_heat *device_in, char *read_buf);

#endif //C_MSC_DCL_HEAT_H
