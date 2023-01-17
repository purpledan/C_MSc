//
// Created by Daniel on 2023/01/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TST_MsgQueue.h"

void TST_MsgQueue(DCL_Queue_type *MsgQ, Msg_data_type *buffer) {
    Msg_data_type *p;

    p = malloc(sizeof(Msg_data_type));
    if (!p) {
        perror("Malloc for new test data");
        abort();
    }

    p->ID = buffer->ID;
    stpncpy(p->message, buffer->message, MSG_LEN);

    DCL_Queue_pushBack(MsgQ, p);
}

int TST_MsgRead(DCL_Queue_type *MsgQ, Msg_data_type *buffer) {
    if (!MsgQ->length) {
        return 0;
    }
    Msg_data_type *p = DCL_Queue_pop(MsgQ);
    if (!p) {
        return -1;
    }

    int l = 0;
    buffer->ID = p->ID;
     l = strlcpy(buffer->message, p->message, MSG_LEN);
    free(p);

    return l;
}

void TST_GenMsg(Msg_data_type *buffer) {
    buffer->ID = rand()%100;

    for (int i = 0; i < MSG_LEN - 1; i++) {
        buffer->message[i] = (char)('A' + (rand()%('Z' - 'A')));
    }
    buffer->message[MSG_LEN - 1] = '\0';
}
