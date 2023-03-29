/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by daniel on 16/01/23. */

#ifndef DCL_MSGQUEUE_H
#define DCL_MSGQUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include "dcl_config.h"

/*  Files: dcl_structs.h & dcl_structs.c
 *  Data structures for Dan's control library
 *  Implements:
 *  Linked List
 *  Message queue
 */

/* Doubly Linked List:
 * Implements a generic double linked list structure.
 *
 */

/** Generic Doubly Linked List */
typedef struct dcl_dList_type {
    struct dcl_dList_type *next;
    struct dcl_dList_type *previous;
    void *data;
}dcl_dList_type;

/* Method Functions */
dcl_dList_type *dcl_dList_create(void *x);
void dcl_dList_elementAdd(dcl_dList_type *dList, void *x);
void *dcl_dList_elementRemove(dcl_dList_type *dList);

/* Message queue:
 * Implements a generic message queue structure.
 * For convenience, a string message system is implemented which
 * may be used for finite state machine inter-thread communication.
 * A pthread mutex and condition is included in the dcl_queue struct,
 * but needs to be initialised by the user along with locking and
 * signaling.
 */

/** String Based Msg element for general use */
typedef struct dcl_strmsg_type {
    char argstr[DCL_STRMSG_LEN];    // Incoming string based message
    bool terminate;                 // May be used to signal queue termination
}dcl_strmsg_type;


/** Generic Message element */
typedef struct dcl_msg_type {
    struct  dcl_msg_type *next;     // The element next in line from the front of the queue
    void    *data;
}dcl_msg_type;

/** Generic Message Queue */
typedef struct dcl_queue {
    pthread_mutex_t mutex;
    pthread_cond_t  alert;
    dcl_msg_type    *first;
    dcl_msg_type    *last;
    size_t          length;
}dcl_queue_type;

/* Method Functions */

/* Initializes message queue
 * mutex & alert is the user's responsibility */
void    dcl_queue_init(dcl_queue_type *MsgQueue);
/* Enqueues x to the back of MsgQueue */
void    dcl_queue_pushBack(dcl_queue_type *MsgQueue, void *x);
/* Enqueues x to the front of MsgQueue */
void    dcl_queue_pushFront(dcl_queue_type *MsgQueue, void *x);
/* Dequeues the front most element from MsgQueue and returns a pointer to it */
void    *dcl_queue_pop(dcl_queue_type *MsgQueue);
void    dcl_queue_free(dcl_queue_type *MsgQueue);
void    dcl_queue_print(dcl_queue_type *MsgQueue);

/* String MSG queue Method Functions */
void dcl_queue_pushStrMsg(dcl_queue_type *MsgQueue, dcl_strmsg_type *buf);
void dcl_queue_pushStrMsg_front(dcl_queue_type *MsgQueue, dcl_strmsg_type *buf);
int dcl_queue_popStrMsg(dcl_queue_type *MsgQueue, dcl_strmsg_type *buf);
#endif //DCL_MSGQUEUE_H
