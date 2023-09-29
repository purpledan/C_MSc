/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 9/19/23. */

#ifndef C_MSC_DCL_ARBITER_H
#define C_MSC_DCL_ARBITER_H

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "dcl_config.h"
#include "dcl_fsm.h"
#include "dcl_time.h"
#include "dcl_sstrings.h"


/* Bitfield defines for arb_fsm status */
#define MSGRDY  0b00000001      // A MSG is ready for action

typedef enum state_arb {
    state_arb_init,
    state_arb_idle,
    state_arb_getMsg,
    state_arb_readMsg,          //TODO: I should probably get a better name for this
    state_arb_terminate,
    state_arb_exit,
    state_arb_critical,
    arb_numStates
} state_arb;

typedef struct arb_msgList {
    int dest_ID;
    char msg[DCL_DCODE_ARGNUM][DCL_DCODE_ARGLEN];
    struct arb_msgList *next;
} arb_msgList;

typedef struct arb_execQueue {
    size_t length;
    arb_msgList *first;
    arb_msgList *last;
}arb_execQueue;

typedef struct arb_cluster {
    dcl_fsm_cluster *triC_fsm;     // Generic FSM cluster for pumps
    dcl_fsm_cluster *arb_fsm;      // FSM cluster for arbiter
    pthread_mutex_t *ext_mutex;         // Link to ext_array status mutex
    arb_execQueue exec_queue;             // Execution queue
    bool enable_external;               // Enable link to ext_array status
    bool enable_log;                    // Enable writing log file
}arb_cluster;

arb_cluster  *state_arbFsm_setup(dcl_queue_type *arb_queue_in, dcl_queue_type *tric_queue_in);
state_arb state_arbFsm_init(arb_cluster *cluster_in);
state_arb state_arbFsm_idle(arb_cluster *cluster_in);
state_arb state_arbFsm_getMsg(arb_cluster *cluster_in);
state_arb state_arbFsm_readMsg(arb_cluster *cluster_in);
/* Queue Helper Functions */
/** Enqueues a msg, returns a pointer to the new block of memory in the queue */
arb_msgList *arb_enqueue(arb_cluster *cluster_in);

/** Dequeues a msg, returns a pointer to the block of memory, requires the user to free the block after use */
arb_msgList *arb_dequeue(arb_cluster *cluster_in);
#endif //C_MSC_DCL_ARBITER_H
