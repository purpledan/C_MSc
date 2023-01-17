//
// Created by Daniel on 2023/01/16.
//

#ifndef C_MSC_TST_MSGQUEUE_H
#define C_MSC_TST_MSGQUEUE_H

#include "DCL_MsgQueue.h"

#define MSG_LEN 32
typedef struct Msg_data {
    int ID;
    char message[MSG_LEN];
} Msg_data_type;

void TST_MsgQueue(DCL_Queue_type *MsgQ, Msg_data_type *buffer);
int TST_MsgRead(DCL_Queue_type *MsgQ, Msg_data_type *buffer);

void TST_GenMsg(Msg_data_type *buffer);
#endif //C_MSC_TST_MSGQUEUE_H
