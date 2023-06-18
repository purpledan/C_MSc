/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 31/01/23.*/

#include "dcl_fsm.h"

int dcl_fsm_getMsg(dcl_fsm_cluster_type *cluster_in) {
    char ret_flag;
    int msg_len;

    if (cluster_in->queue->length) {

        msg_len = dcl_queue_popStrMsg(cluster_in->queue, &cluster_in->msg_buf);
        if (msg_len < 0) {
            ret_flag = MSGERR;       // This is probably unrecoverable
        } else if (cluster_in->msg_buf.terminate) {
            ret_flag = FSMEND;
        } else if (!msg_len) {
            ret_flag = MSGEMP;       // In case empty string was queued
        } else {
            ret_flag = MSGRET;       // Getting MSG was successful
        }
    } else {
        ret_flag = NOMSGS;           // The queue is empty
        cluster_in->queue_empty = true;
    }

    return ret_flag;
}

int dcl_fsm_thr_getMsg(dcl_fsm_cluster_type *cluster_in) {
    pthread_mutex_lock(&cluster_in->queue->mutex);
    int ret_flag = dcl_fsm_getMsg(cluster_in);
    pthread_mutex_unlock(&cluster_in->queue->mutex);
    return ret_flag;
}

int dcl_fsm_thr_timed_getMsg(dcl_fsm_cluster_type *cluster_in) {
    pthread_mutex_lock(&cluster_in->queue->mutex);
    // TODO: pthread_mutex_timedlock(&cluster_in->queue->mutex, );
    int ret_flag = dcl_fsm_getMsg(cluster_in);
    pthread_mutex_unlock(&cluster_in->queue->mutex);
    return ret_flag;
}

int dcl_fsm_terminate(dcl_queue_type *fsm_queue) {
    dcl_strmsg_type buf = {
            .argstr = "",
            .terminate = true
    };
    pthread_mutex_lock(&fsm_queue->mutex);
    dcl_queue_pushStrMsg_front(fsm_queue, &buf);
    pthread_mutex_unlock(&fsm_queue->mutex);
    return 0;
}

int dcl_thr_sendMsg(dcl_queue_type *fsm_queue, dcl_strmsg_type *buf_in) {
    pthread_mutex_lock(&fsm_queue->mutex);
    if ( !fsm_queue->length ) {
        pthread_cond_signal(&fsm_queue->alert);
    }
    dcl_queue_pushStrMsg(fsm_queue, buf_in);
    pthread_mutex_unlock(&fsm_queue->mutex);
    return 0;
}

int dcl_thr_timed_sendMsg(dcl_queue_type *fsm_queue, dcl_strmsg_type *buf_in) {
    pthread_mutex_lock(&fsm_queue->mutex);
    // TODO: pthread_mutex_timedlock(&cluster_in->queue->mutex, );
    dcl_queue_pushStrMsg(fsm_queue, buf_in);
    pthread_mutex_unlock(&fsm_queue->mutex);
    return 0;
}