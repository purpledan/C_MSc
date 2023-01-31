//
// Created by Daniel on 2023/01/23.
//

#ifndef C_MSC_DCL_FSM_TRIKONT_H
#define C_MSC_DCL_FSM_TRIKONT_H
#include <stdbool.h>
#include "dcl_triKont.h"
#include "dcl_fsm.h"

typedef enum state_triC {state_init,
                         state_action,
                         state_transient,
                         state_idle,
                         state_getMsg,
                         state_updateStat,
                         state_exit,
                         numStates
} state_triC;

typedef struct state_triC_cluster {
    dcl_serialDevice *device_in;
    dcl_queue_type *queue;
    msg_type msg_buffer;
    char nxt_cmd[4];
    int arg1;
    int arg2;
    bool action_busy;   // Pump is performing an action
}state_triC_cluster_type;

void state_triC_create(void *arg);

state_triC state_triC_init(state_triC_cluster_type *cluster_in);
state_triC state_triC_idle(state_triC_cluster_type *cluster_in);
state_triC state_triC_getMsg(state_triC_cluster_type *cluster_in);
state_triC state_triC_action(state_triC_cluster_type *cluster_in);
state_triC state_triC_transient(state_triC_cluster_type *cluster_in);
#endif //C_MSC_DCL_FSM_TRIKONT_H
