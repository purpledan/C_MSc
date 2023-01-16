//
// Created by daniel on 16/01/23.
//

#ifndef DCL_MSGQUEUE_H
#define DCL_MSGQUEUE_H

#include <pthread.h>

/** DCL Message Queue
 * Implements a Message queue with embedded support for pthreads
 * Implementing pthreads is not part of this interface, it is the user's responsibility
 *
 */

/* Message element */
typedef struct DCL_Msg {
    struct  DCL_Msg *next;  // The element next in line from the front of the queue
    void    *data;
}DCL_Msg_type;

/* Message Queue */
typedef struct DCL_Queue {
    pthread_mutex_t mutex;
    pthread_cond_t  alert;
    DCL_Msg_type    *first;
    DCL_Msg_type    *last;
    size_t          length;
}DCL_Queue_type;

/* Method Functions */
void            DCL_Queue_init(DCL_Queue_type *MsgQueue);
void            DCL_Queue_pushBack(DCL_Queue_type *MsgQueue, void *x);
void            DCL_Queue_pushFront(DCL_Queue_type *MsgQueue, void *x);
void            *DCL_Queue_pop(DCL_Queue_type *MsgQueue);
void            DCL_Queue_free(DCL_Queue_type *MsgQueue);

#endif //DCL_MSGQUEUE_H
