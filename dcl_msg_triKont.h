//
// Created by dan on 26/01/23.
//

#ifndef C_MSC_DCL_MSG_TRIKONT_H
#define C_MSC_DCL_MSG_TRIKONT_H

#include "dcl_msgQueue.h"

#define MSG_LEN 32

typedef struct msg_type {
    char argstr[MSG_LEN];
    int terminate;
}msg_type;

void triC_queueMsg(dcl_queue_type *msgQ, msg_type *buffer);
int triC_readMsg(dcl_queue_type *msgQ, msg_type *buffer);

#endif //C_MSC_DCL_MSG_TRIKONT_H
