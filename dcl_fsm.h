//
// Created by dan on 31/01/23.
//

#ifndef C_MSC_DCL_FSM_H
#define C_MSC_DCL_FSM_H
#include <pthread.h>
#include <time.h>
#include "dcl_config.h"
#include "dcl_msgQueue.h"

/* Msg Return Definitions*/
enum {  MSGERR, // Pop returned NULL
        MSGEMP, // Popped MSG with empty string
        MSGRET, // Popped MSG with contents
        NOMSGS, // No MSG to pop, ie Queue empty
        FSMEND  // Request for FSM termination
        };

/* FSM General cluster */
typedef struct dcl_fsm_cluster {
    dcl_queue_type      *queue;         // Queue used for FSM communication by main thread
    dcl_strmsg_type     msg_buf;        // Buffer to hold read messages from queue
    char                opt_field;      // Bit field for internal FSM status flags
    bool                queue_empty;    // True if queue is empty, set by getMsg functions
    bool                fsm_terminate;  // True if termination request was received by getMsg
} dcl_fsm_cluster_type;

/** Dequeues message from multithreaded queue
 *
 * @param cluster_in
 * @return
 */
int dcl_fsm_getMsg(dcl_fsm_cluster_type *cluster_in);

int dcl_fsm_thr_getMsg(dcl_fsm_cluster_type *cluster_in);
int dcl_fsm_thr_timed_getMsg(dcl_fsm_cluster_type *cluster_in);

int dcl_thr_sendMsg(dcl_queue_type *fsm_queue, dcl_strmsg_type *buf_in);
int dcl_thr_timed_sendMsg(dcl_queue_type *fsm_queue, dcl_strmsg_type *buf_in);
#endif //C_MSC_DCL_FSM_H
