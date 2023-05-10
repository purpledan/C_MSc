/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by daniel on 16/01/23. */

#include "dcl_structs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

dcl_dList_type *dcl_dList_create(void *x) {
    dcl_dList_type *p;

    p = malloc(sizeof (dcl_dList_type));
    if (!p) {
        perror("Malloc for dList creation failed");
        abort();
    }

    p->next = NULL;
    p->previous = NULL;
    p->data = x;

    return p;
}

void dcl_dList_elementAdd(dcl_dList_type *dList, void *x) {
    dcl_dList_type *p = dList;

    while (p->next) {
        p = p->next;
    }

    p->next = malloc(sizeof (dcl_dList_type));
    if (!p->next) {
        perror("Malloc for dList addition failed");
        abort();
    }

    p->next->next = NULL;
    p->next->previous = p;
}

void *dcl_dList_elementRemove(dcl_dList_type *dList) {
    dcl_dList_type *p = dList;

    if (!p) {
        return NULL;
    }
    void *x = dList->data;

    if (p->next) {
        p->next->previous = p->previous;
    }

    if (p->previous) {
        p->previous->next = p->next;
    }

    free(p);
    return x;
}

void *dcl_dList_elementSearch(dcl_dList_type *dList, void *x, bool (*compare) (const void *, const void *)) {
    dcl_dList_type *p = dList;

    while (p) {
        if ( compare(p, x) ) {
            break;
        }
        p = p->next;
    }

    return p;
}

void dcl_queue_init(dcl_queue_type *MsgQueue) {
    MsgQueue->first     = NULL;
    MsgQueue->last      = NULL;
    MsgQueue->length    = 0;
}

void dcl_queue_pushBack(dcl_queue_type *MsgQueue, void *x) {
    dcl_msg_type *p;

    p = malloc(sizeof(dcl_msg_type));
    if (!p) {
        perror("Malloc for new element in Queue");
        abort();
    }
    if (!MsgQueue->length) {
        MsgQueue->first = p;         // Make new element first element in Queue
        MsgQueue->last = p;          // and last element in Queue
    }

    MsgQueue->last->next = p;
    MsgQueue->last = p;              // Make new element last element in Queue
    p->next = NULL;
    p->data = x;
    MsgQueue->length++;
}

void dcl_queue_pushFront(dcl_queue_type *MsgQueue, void *x) {
    dcl_msg_type *p;

    p = malloc(sizeof(dcl_msg_type));
    if (!p) {
        perror("Malloc for new element in Queue");
        abort();
    }
    if (!MsgQueue->length) {
        MsgQueue->first = p;         // Make new element first element in Queue
        MsgQueue->last = p;          // and last element in Queue
        p->next = NULL;
    }

    p->next = MsgQueue->first;
    MsgQueue->first = p;

    p->data = x;
    MsgQueue->length++;
}

void *dcl_queue_pop(dcl_queue_type *MsgQueue) {
    void *x;
    dcl_msg_type *temp;

    if (!MsgQueue->length) {
        return NULL;
    } else {
        x = MsgQueue->first->data;
        temp = MsgQueue->first->next;
        free(MsgQueue->first);
        MsgQueue->first = temp;
    }
    MsgQueue->length--;
    if (!MsgQueue->length) {
        MsgQueue->first = NULL;
        MsgQueue->last = NULL;
    }
    return x;
}

void dcl_queue_free(dcl_queue_type *MsgQueue) {
    while (MsgQueue->length) {
        dcl_queue_pop(MsgQueue);
    }
}

void dcl_queue_print(dcl_queue_type *MsgQueue) {
    printf("FIRST\n");
    dcl_msg_type *indexer = MsgQueue->first;

    for (int i = 0; i < MsgQueue->length; i++) {
        printf("(%d): %p -> %p\n", i, indexer, indexer->data);
        indexer = indexer->next;
    }
    printf("LAST\n");
}

void dcl_queue_pushStrMsg(dcl_queue_type *MsgQueue, dcl_strmsg_type *buf) {
    dcl_strmsg_type *p;

    p = malloc( sizeof(dcl_strmsg_type) );
    if (!p) {
        perror("Malloc failed for new strmsg in queue");
        abort();
    }

    p->terminate = buf->terminate;
    strncpy(p->argstr, buf->argstr, DCL_STRMSG_LEN - 1);
    dcl_queue_pushBack(MsgQueue, p);
}

void dcl_queue_pushStrMsg_front(dcl_queue_type *MsgQueue, dcl_strmsg_type *buf) {
    dcl_strmsg_type *p;

    p = malloc( sizeof(dcl_strmsg_type) );
    if (!p) {
        perror("Malloc failed for new strmsg in queue");
        abort();
    }

    p->terminate = buf->terminate;
    strncpy(p->argstr, buf->argstr, DCL_STRMSG_LEN - 1);
    dcl_queue_pushFront(MsgQueue, p);
}

int dcl_queue_popStrMsg(dcl_queue_type *MsgQueue, dcl_strmsg_type *buf) {
    if (!MsgQueue->length) {
        return 0;
    }

    dcl_strmsg_type *p = dcl_queue_pop(MsgQueue);
    if (!p) {
        fprintf(stderr, "Read NULL from Msg Queue\n");
        return -1;
    }

    buf->terminate = p->terminate;
    strncpy(buf->argstr, p->argstr, DCL_STRMSG_LEN - 1);
    free(p);

    return (int)strlen(buf->argstr);    //CAST: DCL_STRMSG_LEN is tiny compared to MAX_USIGNED_LONG
}