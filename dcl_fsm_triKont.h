//
// Created by Daniel on 2023/01/23.
//

#ifndef C_MSC_DCL_FSM_TRIKONT_H
#define C_MSC_DCL_FSM_TRIKONT_H
#include "dcl_triKont.h"

enum state_triC {state_init, state_action, state_transient, state_idle, state_getMsg, state_updateStat, numStates};
typedef struct state_triC_cluster {
    dcl_serialDevice *device_in;
    // TODO struct Copy of last MSG
    // TODO 
    // TODO bool waiting for pump to complete action
}state_triC_cluster_type;
void state_triC_create(void *arg);

int state_triC_init(void *arg);
void *state_triC_idle(void *arg);
#endif //C_MSC_DCL_FSM_TRIKONT_H
