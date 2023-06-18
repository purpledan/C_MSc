/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2022/08/23.*/

#include "dcl_serial.h"
#include "dcl_string.h"


int dcl_serial_open(dcl_serialDevice* device_in) {
    device_in->fd = open(device_in->dev_name, O_RDWR | O_NOCTTY | O_NONBLOCK);   // R/W, don't assume control, and don't block
    return device_in->fd;
}

int dcl_serial_close(dcl_serialDevice* device_in) {
    if (close(device_in->fd) < 0 ) {
        fprintf(stderr, "Closing %s Fail: ", device_in->instrument_name);
        perror("");
        return -1;
    }
    return 0;
}

int dcl_serial_conf(dcl_serialDevice* device_in) {
    struct termios tty;

    if ( !tcgetattr(device_in->fd, &tty) ) {
        cfsetspeed(&tty, B9600);
        tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS | CSIZE);
        tty.c_cflag |= (CREAD | CLOCAL | HUPCL | CS8);
        tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
        tty.c_iflag |= (IGNPAR | BRKINT | IGNCR);
        tty.c_oflag &= ~( OPOST | ONLCR | OCRNL);
        tty.c_lflag = ICANON;
        tty.c_lflag &= ~ECHO;

        return tcsetattr(device_in->fd, TCSANOW, &tty);
    }

    return -1;
}

int dcl_serial_setup(dcl_serialDevice* device_in) {
    if ( dcl_serial_open(device_in) < 0 ) {
        fprintf(stderr, "Opening %s Fail: ", device_in->instrument_name);
        perror("");
        return -1;
    }
    if (dcl_serial_conf(device_in) < 0 ) {
        fprintf(stderr, "Configure %s Fail: ", device_in->instrument_name);
        perror("");
        return -1;
    }
    fcntl(device_in->fd, F_SETFL, !O_NONBLOCK);
    tcflush(device_in->fd, TCIOFLUSH);
    return 0;
}

ssize_t dcl_serial_write(dcl_serialDevice* device_in, char* cmd_buffer) {
    size_t cmd_length = strlen(cmd_buffer);
    ssize_t wrote_bytes = write(device_in->fd, cmd_buffer, cmd_length);
    if ( wrote_bytes < 0 ) {
        fprintf(stderr, "CMD to %s Fail: ", device_in->instrument_name);
        perror("");
    }
    return wrote_bytes;
}

ssize_t dcl_serial_read(dcl_serialDevice* device_in, char* ret_buffer, unsigned ret_length) {
    ssize_t read_bytes = read(device_in->fd, ret_buffer, ret_length);
    if ( read_bytes < 0 ) {
        fprintf(stderr, "READ from %s Fail: ", device_in->instrument_name);
        perror("");
    } else {
        ret_buffer[read_bytes] = '\0';
    }
    return read_bytes;
}