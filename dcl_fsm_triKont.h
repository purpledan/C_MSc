/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2023/01/23. */

#ifndef C_MSC_DCL_FSM_TRIKONT_H
#define C_MSC_DCL_FSM_TRIKONT_H
#include <stdbool.h>
#include "dcl_triKont.h"
#include "dcl_fsm.h"

/*  Files: dcl_fsm_triKont.h & dcl_fsm_triKont.c
 *  Implements message queue based finite state machine to control Tricontinent C3000 pumps
 *
 *  Messages are in the format:
 *  Pump-address,command,argument1,argument2
 *  [d,sss,d,dddd]
 *
 *  Possible Commands:
 *  PSH
 *  PUL
 *  CFG
 *  SET
 */

/* Bitfield defines for triC_fsm status */
#define MSGRDY  0b00000001      // A MSG is ready for action
#define ACTBSY  0b00000010      // FSM is busy executing MSG in buffer
#define SPBUSY  0b00000100      // Selected pump is busy

/* Cmd enum */
typedef enum action_triC {
    action_pul,                 // PUL string command
    action_psh,                 // PSH string command
    action_cfg,                 // CFG string command
    action_set,                 // SET string command
    action_err                  // Parse failure
} action_triC;

typedef enum setting_triC {
    setting_speed
}setting_triC;

typedef enum state_triC {
    state_init,
    state_action,
    state_transient,
    state_idle,
    state_getMsg,
    state_halt,
    state_terminate,
    state_exit,
    state_critical,
    numStates
} state_triC;

typedef struct triC_fsm_cluster {
    dcl_fsm_cluster_type *fsm;          // Generic FSM cluster
    dcl_serialDevice_triC *device_in;  // TriC Device in
    pthread_mutex_t *ext_mutex;         // Link to external status mutex
    dcl_triC_status *external;          // Link to external status
    action_triC nxt_cmd;
    int addr_arg;
    int arg1;
    int arg2;
    char state_field;
    bool init_complete;                 // Init completed
    bool enable_external;               // Enable link to external status
    bool enable_log;                    // Enable writing log file
}triC_fsm_cluster;

//void state_triC_create(state_triC *fsm_array);
triC_fsm_cluster *state_triC_fsmSetup(dcl_queue_type *fsm_msg_queue,
                                      dcl_serialDevice_triC *triC_dev,
                                      pthread_mutex_t *ext_mutex,
                                      dcl_triC_status *ext_status);
state_triC state_triC_init(triC_fsm_cluster *cluster_in);
state_triC state_triC_idle(triC_fsm_cluster *cluster_in);
state_triC state_triC_getMsg(triC_fsm_cluster *cluster_in);
state_triC state_triC_action(triC_fsm_cluster *cluster_in);
state_triC state_triC_transient(triC_fsm_cluster *cluster_in);
state_triC state_triC_halt(triC_fsm_cluster *cluster_in);
state_triC state_triC_critical(triC_fsm_cluster *cluster_in);
state_triC state_triC_terminate(triC_fsm_cluster *cluster_in);
int ext_triC_updateStatus(triC_fsm_cluster *cluster_in);
void aux_triC_parseMsg(triC_fsm_cluster *cluster_in);
void action_triC_psh(triC_fsm_cluster *cluster_in);
void action_triC_pul(triC_fsm_cluster *cluster_in);
void action_triC_set(triC_fsm_cluster *cluster_in);
void action_triC_cfg(triC_fsm_cluster *cluster_in);
#endif //C_MSC_DCL_FSM_TRIKONT_H
