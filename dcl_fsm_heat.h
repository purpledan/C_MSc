/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 9/18/23. */

/*  Files: dcl_fsm_heat.h & dcl_fsm_heat.c
 *  Implements message queue based finite state machine to control Radleys Stirring Hotplates
 *
 *  Messages are in the format:
 *  command,argument1,argument2
 *  [sss,d,dddd]
 *
 *  Possible Commands:
 *  PSH
 *  PUL
 *  CFG
 *  SET
 */

#ifndef C_MSC_DCL_FSM_HEAT_H
#define C_MSC_DCL_FSM_HEAT_H

#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "dcl_heat.h"
#include "dcl_fsm.h"
#include "dcl_time.h"

typedef enum action_heat {
    action_heating,             // HTR string command
    action_stirring,            // STR string command
    action_set,                 // CFG string command
    action_get,                 // GET string command
    action_syn,                 // SYN string command
    action_nan,                 // No command
    action_err                  // Parse failure
} action_heat;

typedef enum state_heat {
    state_init,
    state_supervise,
    state_delegate,
    state_resolve,
    state_action,
    state_transient,
    state_idle,
    state_getMsg,
    state_halt,
    state_terminate,
    state_exit,
    state_critical,
    numStates
} state_heat;

typedef struct heat_fsm_buf {
    char dev_flags;                     // Device specific flags
    int addr_arg;                       // Device address
    action_heat nxt_cmd;                // Next command to run
    int arg0;
    int arg1;
    int arg2;
}heat_fsm_buf;

typedef struct heat_fsm_cluster {
    dcl_fsm_cluster *fsm;          // Generic FSM cluster
    dcl_serialDevice_heat *device_in;   // TriC Device in
    pthread_mutex_t *ext_mutex;         // Link to ext_array status mutex
    dcl_heat_status *ext_array;          // Link to ext_array status
    heat_fsm_buf *cmd_array;            // Device specific CMD buffer
    heat_fsm_buf cmd_buf;               // MSG in buffer
    bool init_complete;                 // Init completed
    bool enable_external;               // Enable link to ext_array status
    bool enable_log;                    // Enable writing log file
}heat_fsm_cluster;


#endif //C_MSC_DCL_FSM_HEAT_H
