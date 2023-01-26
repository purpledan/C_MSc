//
// Created by dan on 26/01/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcl_msg_triKont.h"

void triC_queueMsg(dcl_queue_type *msgQ, msg_type *buffer) {
    msg_type *p;

    p = malloc( sizeof(msg_type) );
    if (!p) {
        perror("Malloc fuckup for new msg in queue");
        abort();
    }

    stpncpy(p->arg, buffer->arg, MSG_LEN - 1);
    dcl_queue_pushBack(msgQ, p);
}

int triC_readMsg(dcl_queue_type *msgQ, msg_type *buffer) {
    if (!msgQ->length) {
        return 0;
    }
    msg_type *p = dcl_queue_pop(msgQ);
    if (!p) {
        return -1;
    }

    buffer->terminate = p->terminate;
    strncpy(buffer->arg,p->arg, MSG_LEN - 1);
    free(p);

    return 1;
}
