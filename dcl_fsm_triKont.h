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

typedef struct triC_fsm_cluster {
    dcl_fsm_cluster_type *fsm;          // Generic FSM cluster
    dcl_serialDevice *device_in;        // Generic Device in
    dcl_triC_status *status_in;         // TriC status pointer from device_in
    char nxt_cmd[4];
    int arg1;
    int arg2;
}triC_fsm_cluster;

//state_triC state_triC_create(void *arg);
void state_triC_fsmSetup(triC_fsm_cluster *cluster_in);
state_triC state_triC_init(triC_fsm_cluster *cluster_in);
state_triC state_triC_idle(triC_fsm_cluster *cluster_in);
state_triC state_triC_getMsg(triC_fsm_cluster *cluster_in);
state_triC state_triC_action(triC_fsm_cluster *cluster_in);
state_triC state_triC_transient(triC_fsm_cluster *cluster_in);
#endif //C_MSC_DCL_FSM_TRIKONT_H
