//
// Created by dan on 31/01/23.
//

#ifndef C_MSC_DCL_FSM_H
#define C_MSC_DCL_FSM_H
#include <pthread.h>
#include "dcl_config.h"
#include "dcl_msgQueue.h"

/* Msg Return Definitions*/
enum {  MSGERR, // Pop returned NULL
        MSGEMP, // Popped MSG with empty string
        MSGRET, // Popped MSG with contents
        NOMSGS  // No MSG to pop, ie Queue empty
        };

/* FSM General cluster */
typedef struct dcl_fsm_cluster {
    dcl_queue_type      *queue;         // Queue used for FSM communication by main thread
    dcl_strmsg_type     msg_buf;        // Buffer to hold read messages from queue
    char                opt_field;      // Bit field for internal FSM status flags
} dcl_fsm_cluster_type;

/** Dequeues message from multithreaded queue
 *
 * @param clusterIn
 * @return
 */
int dcl_fsm_getMsg(dcl_fsm_cluster_type *clusterIn);
#endif //C_MSC_DCL_FSM_H
