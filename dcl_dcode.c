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
    cluster_in->step_list = NULL;
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
    } else {
        cluster_in->block = block_step;
    }
    if ( !cluster_in->step_list ) {
        cluster_in->step_list = malloc( sizeof (dcode_triC_steps) );
        if (!cluster_in->step_list) {
            perror("Failed to malloc step: ");
            return state_dcode_abort;
        }
        cluster_in->step_list->next_step = NULL;

        cluster_in->current_step = cluster_in->step_list;
    } else {
        //TODO scan until end of linked list
    }
    cluster_in->current_step->index = 0;
    strcpy(cluster_in->current_step->step_name, &cluster_in->file.line_buf[1]); /* Skip the '!' */

    return state_dcode_scan;
}

state_dcode state_dcodeFsm_blkEnd(dcode_cluster *cluster_in) {
    cluster_in->block = block_outside;
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_config(dcode_cluster *cluster_in) {
    char *argv[DCODE_ARGNO];
    dcode_args args_buf;
    args_buf.line_in = cluster_in->file.line_buf;
    args_buf.argv = argv;
    dcode_config_lexer(&args_buf);

    /* Check reserved args */
    if ( !strcmp(args_buf.argv[0], "SPEED") ) {
        cluster_in->config.default_speed = strtol(args_buf.argv[1], NULL, 10);
    } else {
        int valve_index;
        if ( (valve_index = (int)strtol(args_buf.argv[0], NULL, 10)) ) { // CAST: Index has max value of 6 <<<<<<<<< INT_MAX
            strcpy(cluster_in->config.valve_names[valve_index - 1], args_buf.argv[1]);
            if ( args_buf.argc == 3 ) {
                cluster_in->config.valve_speeds[valve_index - 1] = (int)strtol(args_buf.argv[2], NULL, 10);
            }
        }
    }
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_step(dcode_cluster *cluster_in) {
    char *argv[DCODE_ARGNO];
    dcode_args args_buf;
    args_buf.line_in = cluster_in->file.line_buf;
    args_buf.argv = argv;
    dcode_step_lexer(&args_buf);
    // TODO: Abstract creation of STRMSGs
    int valve;
    if (args_buf.argc == 1) {
        valve = dcode_search_valve(cluster_in, args_buf.argv[0]);
        sprintf(cluster_in->current_step->block[cluster_in->current_step->index], "SET,%d,0", valve);
        cluster_in->current_step->index++;
    } else if (args_buf.argc == 2) {
        valve = dcode_search_valve(cluster_in, args_buf.argv[0]);
        sprintf(cluster_in->current_step->block[cluster_in->current_step->index], "PUL,%d,3000", valve);
        cluster_in->current_step->index++;
        valve = dcode_search_valve(cluster_in, args_buf.argv[1]);
        sprintf(cluster_in->current_step->block[cluster_in->current_step->index], "PSH,%d,3000", valve);
        cluster_in->current_step->index++;
    }
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

void dcode_step_lexer(dcode_args *args_in) {
    int index= 0;
    args_in->argv[index] = args_in->line_in;
    index++;

    char *string_p = strstr(args_in->line_in, "->");
    if (string_p) {
        memset(string_p, '\0', sizeof ("->") - 1);
        string_p += sizeof ("->") - 1;
        args_in->argv[index] = string_p;
        index++;
    } else {
        string_p = args_in->line_in;
    }

    while (*string_p) {
        if (*string_p == ';') {
            *string_p = '\0';
            string_p++;
            args_in->argv[index] = string_p;
            index++;
        } else if (*string_p == '\n') {
            *string_p = '\0';
            break;
        }
        string_p++;
    }
    args_in->argc = index;
}

void dcode_config_lexer(dcode_args *args_in) {
    int index= 0;
    args_in->argv[index] = args_in->line_in;
    index++;

    char *string_p = args_in->line_in;
    while (*string_p) {
        if (*string_p == ';' || *string_p == '=') {
            *string_p = '\0';
            string_p++;
            args_in->argv[index] = string_p;
            index++;
        } else if (*string_p == '\n') {
            *string_p = '\0';
            break;
        }
        string_p++;
    }
    args_in->argc = index;
}

int dcode_search_valve(dcode_cluster *cluster_in, char *name) {
    for (int i = 0; i < DCL_TRIC_VALVENO; i++) {
        if ( !strcmp(name, cluster_in->config.valve_names[i]) ) {
            return i + 1;
        }
    }
    return 0;
}