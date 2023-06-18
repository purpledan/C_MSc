/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2022/08/23.*/

#ifndef INSTRUMENTMAN_DCL_SERIAL_H
#define INSTRUMENTMAN_DCL_SERIAL_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define DEV_NAME_LEN 64
#define INSTRUMENT_NAME_LEN 64

typedef struct dcl_serialDevice{
    char instrument_name[INSTRUMENT_NAME_LEN];  /* User Friendly instrument name */
    char dev_name[DEV_NAME_LEN];                /* Serial Device name */
    void *dev_status;                           /* Generic Device Status */
    int fd;                                     /* File Descriptor */
}dcl_serialDevice;

/** Opens serial port to instrument
 *
 * @param device_name
 */
int dcl_serial_open(dcl_serialDevice* device_name);

/** Closes serial port to instrument
 *
 * @param device_in
 */
int dcl_serial_close(dcl_serialDevice* device_in);

/** Configures serial port for VISA type communication
 *
 * @param fd
 * @return Returns 0 if successful, else -1
 */
int dcl_serial_conf(dcl_serialDevice* device_in);

/** Complete setup of an instrument
 *
 */

int dcl_serial_setup(dcl_serialDevice* device_in);

ssize_t dcl_serial_write(dcl_serialDevice* device_in, char* cmd_buffer);
ssize_t dcl_serial_read(dcl_serialDevice* device_in, char* ret_buffer, unsigned ret_length);

#endif //INSTRUMENTMAN_DCL_SERIAL_H
