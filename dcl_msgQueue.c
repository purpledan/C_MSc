//
// Created by daniel on 16/01/23.
//

#include "dcl_msgQueue.h"
#include <stdlib.h>
#include <stdio.h>

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