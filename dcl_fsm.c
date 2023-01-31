//
// Created by dan on 31/01/23.
//

#include "dcl_fsm.h"

int dcl_fsm_getMsg(dcl_fsm_cluster_type *clusterIn) {
    char retFlag;
    int msg_len;
    pthread_mutex_lock(&clusterIn->queue->mutex);

    if (clusterIn->queue->length) {

        msg_len = dcl_queue_popStrMsg(clusterIn->queue, &clusterIn->msg_buf);
        if (msg_len < 0) {
            retFlag = MSGERR;       // This is probably unrecoverable
        } else if (!msg_len) {
            retFlag = MSGEMP;       // In case empty string was queued
        } else {
            retFlag = MSGRET;       // Getting MSG was successful
        }
    } else {
        retFlag = NOMSGS;           // The queue is empty
    }
    pthread_mutex_unlock(&clusterIn->queue->mutex);
    return retFlag;
}
