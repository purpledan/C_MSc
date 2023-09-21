/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 9/19/23. */

#ifndef C_MSC_DCL_ARBITER_H
#define C_MSC_DCL_ARBITER_H

#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "dcl_fsm.h"
#include "dcl_time.h"


/* Bitfield defines for arb_fsm status */
#define MSGRDY  0b00000001      // A MSG is ready for action

typedef enum state_arb {
    state_arb_init,
    state_arb_idle,
    state_arb_getMsg,
    state_arb_terminate,
    state_arb_exit,
    state_arb_critical,
    arb_numStates
} state_arb;

typedef struct arb_msgBuf {
    int dest_ID;
    char msg[DCL_STRMSG_LEN];
    struct arb_msgBuf *next_msg;
} arb_msgBuf;

typedef struct arb_cluster {
    dcl_fsm_cluster *triC_fsm;     // Generic FSM cluster for pumps
    dcl_fsm_cluster *arb_fsm;      // FSM cluster for arbiter
    pthread_mutex_t *ext_mutex;         // Link to ext_array status mutex
    bool enable_external;               // Enable link to ext_array status
    bool enable_log;                    // Enable writing log file
}arb_cluster;

arb_cluster  *state_arbFsm_setup(dcl_queue_type *arb_queue_in, dcl_queue_type *tric_queue_in);
state_arb state_arbFsm_init(arb_cluster *cluster_in);
state_arb state_arbFsm_idle(arb_cluster *cluster_in);
state_arb state_arbFsm_getMsg(arb_cluster *cluster_in);
#endif //C_MSC_DCL_ARBITER_H
