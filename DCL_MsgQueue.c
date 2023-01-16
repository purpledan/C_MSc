//
// Created by daniel on 16/01/23.
//

#include "DCL_MsgQueue.h"
#include <stdlib.h>
#include <stdio.h>

void DCL_Queue_init(DCL_Queue_type *MsgQueue) {
    MsgQueue->first     = NULL;
    MsgQueue->last      = NULL;
    MsgQueue->length    = 0;
}

void DCL_Queue_pushBack(DCL_Queue_type *MsgQueue, void *x) {
    DCL_Msg_type *p;

    p = malloc(sizeof(DCL_Msg_type));
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

void DCL_Queue_pushFront(DCL_Queue_type *MsgQueue, void *x) {
    DCL_Msg_type *p;

    p = malloc(sizeof(DCL_Msg_type));
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

void *DCL_Queue_pop(DCL_Queue_type *MsgQueue) {
    void *x;
    DCL_Msg_type *temp;

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

void DCL_Queue_free(DCL_Queue_type *MsgQueue) {
    while (MsgQueue->length) {
        DCL_Queue_pop(MsgQueue);
    }
}

void DCL_Queue_print(DCL_Queue_type *MsgQueue) {
    printf("FIRST\n");
    DCL_Msg_type *indexer = MsgQueue->first;

    for (int i = 0; i < MsgQueue->length; i++) {
        printf("(%d): %p -> %p\n", i, indexer, indexer->data);
        indexer = indexer->next;
    }
    printf("LAST\n");
}