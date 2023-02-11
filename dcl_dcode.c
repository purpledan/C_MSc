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
    char temp_string[DCL_DCODE_MAXLINE_LEN];
    if ( !fgets(temp_string, DCL_DCODE_MAXLINE_LEN - 1, cluster_in->file.file_pointer) ) {
        cluster_in->fsm->opt_field |= SCNCMP;
        return state_dcode_exit;
    }
    dcode_rem_wSpace(cluster_in->file.line_buf, temp_string);
    dcode_rem_comments(cluster_in->file.line_buf);

    printf("Read: %s\n", temp_string);
    printf("to: %s\n", cluster_in->file.line_buf);

    cluster_in->file.line_no++;

    switch (*cluster_in->file.line_buf) {
        case '!':
            return state_dcode_blkStart;
        case '@':
            return state_dcode_blkEnd;
        default:
            break;
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
    if ( strstr(cluster_in->file.line_buf, "_CONFIG") ) {
        cluster_in->block = block_config;
        return state_dcode_scan;
    }
    char temp_name[DCL_DCODE_NAME_LEN];
    sscanf(cluster_in->file.line_buf, "!%[A-Z1-9-]", temp_name);
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
    if ( (cluster_in->file.selector = strstr(cluster_in->file.line_buf, "SPEED=")) ) {
        sscanf(cluster_in->file.selector, "SPEED=%d", &cluster_in->config.default_speed);
    } else {
        int temp_valve, temp_speed;
        char temp_name[DCL_DCODE_NAME_LEN];
        /* TODO: Need to use a method that can tell if ",%d" failed */
        sscanf(cluster_in->file.line_buf, "%d=%[A-Z];%d", &temp_valve, temp_name, &temp_speed);
        strcpy(cluster_in->config.valve_names[temp_valve - 1], temp_name);
        cluster_in->config.valve_speeds[temp_valve - 1] = temp_speed;
    }
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_step(dcode_cluster *cluster_in) {
    dcode_step_str args_buffer;
    char *arg_p[DCODE_ARGNO];
    args_buffer.line_in = cluster_in->file.line_buf;
    args_buffer.argv = arg_p;

    dcode_step_lexer(&args_buffer);

    return state_dcode_scan;
}

void dcode_rem_wSpace(char* restrict line_out, const char* restrict line_in) {
    while (*line_in) {
        if ( !isblank(*line_in) ) {
            *line_out = *line_in;
            line_out++;
        }
        line_in++;
    }
    *line_out = '\0';
}
void dcode_rem_comments(char *line_in) {
    char *string_p = strchr(line_in, '#');
    if (string_p) {
        *string_p = '\0';
    }
}

void dcode_step_lexer(dcode_step_str *args_in) {
    int indexer= 0;
    args_in->argv[indexer] = args_in->line_in;
    indexer++;

    char *string_p = strstr(args_in->line_in, "->");
    if (string_p) {
        memset(string_p, '\0', sizeof ("->") - 1);
        string_p += sizeof ("->") - 1;
        args_in->argv[indexer] = string_p;
        indexer++;
    } else {
        string_p = args_in->line_in;
    }

    while (*string_p) {
        if (*string_p == ';') {
            *string_p = '\0';
            string_p++;
            args_in->argv[indexer] = string_p;
            indexer++;
        } else if (*string_p == '\n') {
            *string_p = '\0';
            break;
        }
        string_p++;
    }
    args_in->argc = indexer;
}

