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
typedef struct dcl_msg {
    struct  dcl_msg *next;  // The element next in line from the front of the queue
    void    *data;
}dcl_msg_type;

/* Message Queue */
typedef struct dcl_queue {
    pthread_mutex_t mutex;
    pthread_cond_t  alert;
    dcl_msg_type    *first;
    dcl_msg_type    *last;
    size_t          length;
    char            status;
}dcl_queue_type;

/* Method Functions */
void    dcl_queue_init(dcl_queue_type *MsgQueue);
void    dcl_queue_pushBack(dcl_queue_type *MsgQueue, void *x);
void    dcl_queue_pushFront(dcl_queue_type *MsgQueue, void *x);
void    *dcl_queue_pop(dcl_queue_type *MsgQueue);
void    dcl_queue_free(dcl_queue_type *MsgQueue);
void    dcl_queue_print(dcl_queue_type *MsgQueue);

#endif //DCL_MSGQUEUE_H
