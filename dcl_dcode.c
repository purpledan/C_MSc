//
// Created by dan on 08/02/23.
//

#include "dcl_dcode.h"

dcode_cluster *state_dcodeFsm_setup(char *file_name, dcl_queue_type *queue_in) {
    static dcl_fsm_cluster_type fsm_cluster;
    fsm_cluster.queue = queue_in;

    static dcode_cluster thread_cluster;
    thread_cluster.fsm = &fsm_cluster;
    strcpy(thread_cluster.file.file_name, file_name);
    thread_cluster.file.file_pointer = fopen(thread_cluster.file.file_name, "r");
    if (!thread_cluster.file.file_pointer) {
        perror("Could not open dcode: ");
        abort();
    }
    return &thread_cluster;
}

state_dcode state_dcodeFsm_init(dcode_cluster *cluster_in) {
    cluster_in->file.line_no = 0;
    cluster_in->stepNo = 0;
    cluster_in->fsm->opt_field = '\0';
    cluster_in->block = block_outside;
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_scan(dcode_cluster *cluster_in) {
    char *string_p = NULL;
    string_p = fgets(cluster_in->file.line_buf,
                     DCL_DCODE_MAXLINE_LEN - 1,
                     cluster_in->file.file_pointer);
    cluster_in->file.line_no++;
    if (!string_p) {
        cluster_in->fsm->opt_field |= SCNCMP;
        return state_dcode_exit;
    } else if ( strchr(string_p, '#') ) {
        return state_dcode_scan;
    }
    printf("%s", string_p);

    if ( (cluster_in->file.selector = strchr(string_p, '!')) ) {
        return state_dcode_blkStart;
    } else if ( (cluster_in->file.selector = strchr(string_p, '@')) ) {
        return state_dcode_exit;
    }

    switch (cluster_in->block) {
        case block_config:
            return state_dcode_config;
        case block_step:
            return state_dcode_step;
        case block_run:
            return state_dcode_run;
        case block_outside:
        default:
            return state_dcode_scan;
    }
}

state_dcode state_dcodeFsm_blkStart(dcode_cluster *cluster_in) {
    if ( strstr(cluster_in->file.selector, "_CONFIG") ) {
        cluster_in->block = block_config;
        return state_dcode_scan;
    }
    char temp_name[DCL_DCODE_NAME_LEN];
    sscanf(cluster_in->file.selector, "!%[A-Z1-9-]", temp_name);
    strcpy(cluster_in->steps[cluster_in->stepNo], temp_name);
    if ( !fgetpos(cluster_in->file.file_pointer, &cluster_in->file.step_pos[cluster_in->stepNo]) ) {
        perror("Failed to get filepos: ");
        return state_dcode_abort;
    }
    cluster_in->stepNo++;
    cluster_in->block = block_step;
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_blkEnd(dcode_cluster *cluster_in) {
    cluster_in->block = block_outside;
    return state_dcode_scan;
}
state_dcode state_dcodeFsm_config(dcode_cluster *cluster_in) {
    if ( (cluster_in->file.selector = strstr(cluster_in->file.line_buf, "SPEED =")) ) {
        sscanf(cluster_in->file.selector, "SPEED = %d", &cluster_in->config.default_speed);
    } else {
        int temp_valve, temp_speed;
        char temp_name[DCL_DCODE_NAME_LEN];
        sscanf(cluster_in->file.line_buf, "%d = %[A-Z],%d", &temp_valve, temp_name, &temp_speed);
        strcpy(cluster_in->config.valve_names[temp_valve - 1], temp_name);
        cluster_in->config.valve_speeds[temp_valve - 1] = temp_speed;
    }
    return state_dcode_scan;
}
/*
dcode_block dcode_parse_label(dcode_cluster *cluster_in, char *selection) {
    char flag,
    sscanf(selection, )

}
 */