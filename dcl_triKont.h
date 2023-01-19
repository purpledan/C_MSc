//
// Created by Daniel on 2022/11/23.
//

#ifndef LIB_DCL_TRIKONT_H
#define LIB_DCL_TRIKONT_H

#include <stdbool.h>

#include "dcl_string.h"
#include "dcl_serial.h"

#define TRIK_TERM "\r\n"

enum trik_cmds {init, valve, mov, resol, slope, vel, speed, term, status};
enum trik_args {none, cw, ccw, query, inlet, outlet, absol, asper, disp, plunger, qValve};

typedef struct dcl_trik_status{
    int valve;                      // Valve position
    int plunger;                    // Plunger position
    char statusByte;                // Status Byte
} dcl_trik_status;

typedef struct dcl_trik_cond{
    int address;                    // Switch setting on pump
    char statusByte;                // Queried status byte
}dcl_trik_cond;

int dcl_trik_cmd(dcl_serialDevice* device_in, enum trik_cmds cmd, enum trik_args, int val, struct dcl_string_box* box_in);

int dcl_trik_read(dcl_serialDevice* device_in, dcl_string_box* box_in);
#endif //LIB_DCL_TRIKONT_H
