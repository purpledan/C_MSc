/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 9/19/23. */

#ifndef C_MSC_DCL_SUPERVISOR_H
#define C_MSC_DCL_SUPERVISOR_H

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "dcl_config.h"
#include "dcl_fsm.h"
#include "dcl_time.h"
#include "dcl_sstrings.h"


/* Bitfield defines for spv_fsm status */
#define MSGRDY  0b00000001      // A MSG is ready for action

typedef enum state_spv {
    state_spv_init,
    state_spv_idle,
    state_spv_getJob,
    state_spv_procJob,
    state_spv_allocJob,
    state_arb_readMsg,          //TODO: I should probably get a better name for this
    state_arb_sendMsg,
    state_spv_term,
    state_arb_exit,
    state_spv_crit,
    arb_numStates
} state_spv;

typedef struct job_msg {
    int dest_ID;
    char msg[DCL_DCODE_ARGNUM][DCL_DCODE_ARGLEN];
    struct job_msg *next;
} job_msg;

typedef struct spv_jobQueue {
    size_t length;
    job_msg *first;
    job_msg *last;
}spv_jobQueue;

typedef struct spv_cluster {
    dcl_fsm_cluster *triC_fsm;          // Generic FSM cluster for pumps
    dcl_fsm_cluster *spv_fsm;           // FSM cluster for arbiter
    state_spv prev_state;               // Previous action of idle state
    pthread_mutex_t *ext_mutex;         // Link to ext_array status mutex
    spv_jobQueue staging_queue;         // Staging queue for incoming jobs
    spv_jobQueue sp_queue;              // Job queue for syringe pumps
    spv_jobQueue magStir_queue;         // Job queue for mag stirrer
    bool enable_external;               // Enable link to ext_array status
    bool enable_log;                    // Enable writing log file
}spv_cluster;

spv_cluster  *state_spvFsm_setup(dcl_queue_type *arb_queue_in, dcl_queue_type *tric_queue_in);
state_spv state_spvFsm_init(spv_cluster *cluster_in);
/** The name of this state is slightly misleading, it doesn't just idle, it idles while making a decision. */
/** Broadly there are a hand full job the supervisor needs to do */
state_spv state_spvFsm_idle(spv_cluster *cluster_in);
/** Collect MSG sent to arbiter thread */
state_spv state_spvFsm_getJob(spv_cluster *cluster_in);
/** Send MSG to other threads */
state_spv state_arbFsm_procMsg(spv_cluster *cluster_in);
/** Parse MSG sent to arbiter thread, store if needed */
state_spv state_spvFsm_procJob(spv_cluster *cluster_in);

/* Helper Functions */
/** Wrapper for get MSG */
state_spv spv_getMsg(dcl_fsm_cluster *fsm_in);
/** Enqueues a msg, returns a pointer to the new block of memory in the queue */
job_msg *arb_enqueue(spv_cluster *cluster_in);

/** Dequeues a msg, returns a pointer to the block of memory, requires the user to free the block after use */
job_msg *arb_dequeue(spv_cluster *cluster_in);
#endif //C_MSC_DCL_SUPERVISOR_H
