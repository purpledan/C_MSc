//
// Created by Daniel on 2022/11/23.
//

#include "dcl_triKont.h"

int dcl_trik_cmd(dcl_serialDevice* device_in, enum trik_cmds cmd, enum trik_args arg, int val, dcl_string_box* box_in) {
    /* '/' is needed for cmd start, then pick out dev_status from the void pointer
     * in order to populate address label. */
    char cmd_buffer[32] = "";
    dcl_trik_cond dev_status = *(dcl_trik_cond *)(device_in->dev_status);
    sprintf(cmd_buffer, "/%d", dev_status.address + 1); // switch setting on pump needs +1 to work
    bool rq_send = true;
    // TODO: Make this not ugly!

    switch (cmd) {

        case init:
            if (arg == cw) {
                strcat(cmd_buffer, "ZR" TRIK_TERM);
            } else if (arg == ccw) {
                strcat(cmd_buffer, "YR" TRIK_TERM);
            }
            break;
        case valve:
            if (arg == inlet) {
                sprintf(cmd_buffer, "%sI%dR", cmd_buffer, val);
            } else if (arg == outlet) {
                sprintf(cmd_buffer, "%sO%dR", cmd_buffer, val);
            }
            strcat(cmd_buffer, TRIK_TERM);
            break;
        case mov:
            if (arg == absol) {
                sprintf(cmd_buffer, "A%dR", val);
            } else if (arg == asper) {
                sprintf(cmd_buffer, "P%dR", val);
            } else if (arg == disp) {
                sprintf(cmd_buffer, "D%dR", val);
            }
            strcat(cmd_buffer, TRIK_TERM);
            break;
        case status:
            if (arg == query) {
                sprintf(cmd_buffer, "%sQ", cmd_buffer);
            } else if (arg == plunger) {
                sprintf(cmd_buffer, "%sRZ", cmd_buffer);
            } else if (arg == qValve) {
                sprintf(cmd_buffer, "%s?6", cmd_buffer);
            }
            strcat(cmd_buffer, TRIK_TERM);
            break;
        default:
            rq_send = false;
            break;

    }
    printf("%s\n", cmd_buffer);
    if (rq_send) {
        dcl_serial_write(device_in, cmd_buffer);
    } else {
        return -1;
    }

    dcl_trik_read(device_in, box_in);

    return 0;
}

int dcl_trik_read(dcl_serialDevice* device_in, dcl_string_box* box_in) {
    dcl_string_box_empty(box_in);
    char read_buffer[STATIC_SIZE] = "";
    ssize_t read_len = 0;

    read_len = dcl_serial_read(device_in, read_buffer, STATIC_SIZE -1);
    if (read_len < 0) {
        return -1;
    }
    dcl_string_box_insert(box_in, read_buffer, read_len);
    return 0;
}