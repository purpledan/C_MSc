//
// Created by Daniel on 2023/01/23.
//

#ifndef C_MSC_DCL_FSM_TRIKONT_H
#define C_MSC_DCL_FSM_TRIKONT_H
#include <stdbool.h>
#include "dcl_triKont.h"
#include "dcl_fsm.h"
/* Bitfield defines for internal status */
#define SPBUSY  0b00000001      // Syringe pump is busy performing an action
#define QEMPTY  0b00000010      // Queue empty
#define LSTACT  0b00000100      // Action performing last move
#define INCPLT  0b00001000      // Init state complete
#define MSGRDY  0b00010000      // A MSG is ready for action
#define ARGSEL  0b00100000      // True for Valve Mov, False for Plunger Mov
#define ACTBSY  0b01000000      // FSM is busy executing MSG in buffer

typedef enum state_triC {state_init,
                         state_action,
                         state_transient,
                         state_idle,
                         state_getMsg,
                         state_updateStat,
                         state_terminate,
                         state_exit,
                         state_critical,
                         numStates
} state_triC;

typedef struct triC_fsm_cluster {
    dcl_fsm_cluster_type *fsm;          // Generic FSM cluster
    dcl_serialDevice *device_in;        // Generic Device in
    dcl_triC_status *status_in;         // TriC status pointer from device_in
    pthread_mutex_t *ext_mutex;         // Link to external status mutex
    dcl_triC_status *external;          // Link to external status
    char nxt_cmd[4];
    int arg1;
    int arg2;
    bool enable_external;               // Enable link to external status
}triC_fsm_cluster;

//state_triC state_triC_create(void *arg);
triC_fsm_cluster *state_triC_fsmSetup(dcl_queue_type *fsm_msg_queue,
                                      dcl_serialDevice *triC_dev,
                                      pthread_mutex_t *ext_mutex,
                                      dcl_triC_status *ext_status);
state_triC state_triC_init(triC_fsm_cluster *cluster_in);
state_triC state_triC_idle(triC_fsm_cluster *cluster_in);
state_triC state_triC_getMsg(triC_fsm_cluster *cluster_in);
state_triC state_triC_action(triC_fsm_cluster *cluster_in);
state_triC state_triC_transient(triC_fsm_cluster *cluster_in);
state_triC state_triC_critical(triC_fsm_cluster *cluster_in);
state_triC state_triC_terminate(triC_fsm_cluster *cluster_in);
int ext_triC_updateStatus(triC_fsm_cluster *cluster_in);
void aux_triC_parseMsg(triC_fsm_cluster *cluster_in);
#endif //C_MSC_DCL_FSM_TRIKONT_H
