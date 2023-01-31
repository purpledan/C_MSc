//
// Created by dan on 31/01/23.
//

#include "dcl_fsm.h"

char dcl_fsm_getMsg(dcl_fsm_cluster_type *clusterIn) {
    char retFlag;
    int msg_len;
    pthread_mutex_lock(&clusterIn->queue->mutex);

    if (clusterIn->queue->length) {

        msg_len = dcl_queue_popStrMsg(clusterIn->queue, &clusterIn->msg_buf);
        if (msg_len < 0) {
            retFlag = '\0';    //TODO: return proper char warning
        } else if (!msg_len) {
            retFlag = 'E';     //TODO: return proper warning for empty msg
        } else {
            retFlag = 'O';     //TODO: return proper flag for happy msg
        }
    } else {
        retFlag = 'E';         //TODO: return proper warning for no msg available
    }
    pthread_mutex_unlock(&clusterIn->queue->mutex);
    return retFlag;
}
