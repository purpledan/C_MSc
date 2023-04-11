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
    cluster_in->fsm->opt_field = '\0';
    cluster_in->block = block_outside;
    cluster_in->step_list = NULL;
    cluster_in->config_list = NULL;
    //cluster_in->config.pump_resolution = 3000;      //TODO: Make non magic number
    //cluster_in->config.syringe_volume = 5.0;        //TODO: Also make non magic number
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
    if ( strstr(cluster_in->file.line_buf, "_PUMP_") ) {
        if (!cluster_in->config_list) {
            cluster_in->config_list = malloc( sizeof (dcode_triC_config) );
            if (!cluster_in->config_list) {
                perror("Failled to malloc config: ");
                return state_dcode_abort;
            }
            cluster_in->current_config = cluster_in->config_list;
        } else {
            while (cluster_in->current_config->next_pump) {
                cluster_in->current_config = cluster_in->current_config->next_pump;
            }
            cluster_in->current_config->next_pump = malloc( sizeof (dcode_triC_config) );
            cluster_in->current_config = cluster_in->current_config->next_pump;
            if (!cluster_in->current_config) {
                perror("Failled to malloc config: ");
                return state_dcode_abort;
            }
        }
        cluster_in->current_config->next_pump = NULL;
        cluster_in->block = block_config;
        return state_dcode_scan;
    } else if ( strstr(cluster_in->file.line_buf, "RUN") ) {
        cluster_in->block = block_run;
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
        while (cluster_in->current_step->next_step) {
            cluster_in->current_step = cluster_in->current_step->next_step;
        }
        cluster_in->current_step->next_step = malloc(sizeof (dcode_triC_steps) );
        cluster_in->current_step = cluster_in->current_step->next_step;
        if (!cluster_in->current_step) {
            perror("Failed to malloc step: ");
            return state_dcode_abort;
        }
        cluster_in->current_step->next_step = NULL;
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
        // TODO: Add unit conversion
        cluster_in->current_config->default_speed = (int) strtol(args_buf.argv[1], NULL, 10);
    } else if ( !strcmp(args_buf.argv[0], "ADDRESS") ) {
        cluster_in->current_config->pump_address = (int) strtol(args_buf.argv[1], NULL, 10);
    } else if ( !strcmp(args_buf.argv[0], "NAME") ) {
        strcpy(cluster_in->current_config->pump_name, args_buf.argv[1]);
    } else {
        int valve_index;
        if ( (valve_index = (int)strtol(args_buf.argv[0], NULL, 10)) ) { // CAST: Index has max value of 6 <<<<<<<<< INT_MAX
            strcpy(cluster_in->current_config->valve_names[valve_index - 1], args_buf.argv[1]); // -1 since people count valves starting from 1
            if ( args_buf.argc == 3 ) {
                // TODO: Add unit conversion
                cluster_in->current_config->valve_speeds[valve_index - 1] = (int)strtol(args_buf.argv[2], NULL, 10);
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
    if (args_buf.argc < 2) {
        if ( !strcmp(args_buf.argv[0], "SYNC") ) {
            sprintf(cluster_in->current_step->block[cluster_in->current_step->index],
                    "1,SYN,1,1");
            cluster_in->current_step->index++;
            return state_dcode_scan;
        } else {
            fprintf(stderr,
                    "Line: %d, not enough arguments, did you forget '->'?",
                    (int)cluster_in->file.line_no);
            return state_dcode_abort;
        }
    }
    dcode_valve valve_in, valve_out;
    int amount;
    double read_amount;
    if (*args_buf.argv[0]) {
        valve_in = dcode_search_valve(cluster_in, args_buf.argv[0]);
        if (!valve_in.valid) {
            fprintf(stderr,"Line %d, Input valve not found\n", (int)cluster_in->file.line_no);
            return state_dcode_abort;
        }
    } else {
        valve_in.pump_no = 0;
        valve_in.valve_no = 0;
    }
    if (*args_buf.argv[1]) {
        valve_out = dcode_search_valve(cluster_in, args_buf.argv[1]);
        if (!valve_out.valid) {
            fprintf(stderr,"Line %d, Output valve not found\n", (int)cluster_in->file.line_no);
            return state_dcode_abort;
        }
    } else {
        valve_out.pump_no = 0;
        valve_out.valve_no = 0;
    }
    if ( !(valve_in.valid) && !(valve_out.valid) ) {
        fprintf(stderr,
                "Line: %d, no valve names found, aborting!\n",
                (int)cluster_in->file.line_no);
        return state_dcode_abort;
    }
    if (args_buf.argc >= 3) {
        char *unit;
        read_amount = strtod(args_buf.argv[2], &unit);
        if (*unit) {
            dcode_unit read_unit = dcode_search_unit(unit);
            amount = dcode_unit_convert(cluster_in, read_amount, read_unit);
        } else {
            amount = dcode_unit_convert(cluster_in, read_amount, unit_pts);
        }
    } else {
        amount = 3000;  // Assuming the full 5ml syringe
    }
    if (valve_in.valid) {
        sprintf(cluster_in->current_step->block[cluster_in->current_step->index],
                "%d,PUL,%d,%d",
                valve_in.pump_no,
                valve_in.valve_no,
                amount);
        cluster_in->current_step->index++;
    }
    if (valve_out.valid) {
        sprintf(cluster_in->current_step->block[cluster_in->current_step->index],
                "%d,PSH,%d,%d",
                valve_out.pump_no,
                valve_out.valve_no,
                amount);
        cluster_in->current_step->index++;
    }
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_run(dcode_cluster *cluster_in) {
    cluster_in->current_step = cluster_in->step_list;
    bool step_found = false;
    while (!step_found) {
        if ( !strcmp(cluster_in->file.line_buf, cluster_in->current_step->step_name) ) {
            step_found = true;
        } else {
            if (!cluster_in->current_step->next_step) {
                fprintf(stderr,
                        "Line: %d, could not find step: %s\n",
                        (int)cluster_in->file.line_no,
                        cluster_in->file.line_buf);
                return state_dcode_abort;
            } else {
                cluster_in->current_step = cluster_in->current_step->next_step;
            }
        }
    }
    dcl_strmsg_type buffer = {
            .terminate = 0,
    };

    for (int i = 0; i < cluster_in->current_step->index; i++) {
        strcpy(buffer.argstr, cluster_in->current_step->block[i]);
        dcl_thr_sendMsg(cluster_in->fsm->queue, &buffer);
    }
    return state_dcode_scan;
}

state_dcode state_dcodeFsm_abort(dcode_cluster *cluster_in) {
    fprintf(stderr, "Aborting dcode execution, you are on your own now\n");
    return state_dcode_exit;
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

dcode_valve dcode_search_valve(dcode_cluster *cluster_in, char *name) {
    dcode_valve ret_valve;
    ret_valve.valid = false;
    dcode_triC_config *selector = cluster_in->config_list;
    while (selector) {
        for (int i = 0; i < DCL_TRIC_VALVENO; i++) {
            if ( !strcmp(name, selector->valve_names[i]) ) {
                ret_valve.pump_no = selector->pump_address;
                ret_valve.valve_no = i + 1;
                ret_valve.valid = true;
                selector = NULL;
                break;
            }
        }
        if (selector) {
            selector = selector->next_pump;
        }
    }

    return ret_valve;
}

dcode_unit dcode_search_unit(char *unit_in) {
    // TODO: Make case insensitive
    if ( !strcmp(unit_in, "ML") ) {
        return unit_ml;
    } else if ( !strcmp(unit_in, "UL") ) {
        return unit_ul;
    }  else if ( !strcmp(unit_in, "MLPS") ) {
        return unit_mlps;
    }  else if ( !strcmp(unit_in, "MLPM") ) {
        return unit_mlpm;
    } else {
        return unit_nan;
    }
}

int dcode_unit_convert(dcode_cluster *cluster_in, double amount, dcode_unit unit) {
    switch (unit) {
        case unit_ml:
            return (int)lround( (amount * 600.0) ); // CAST: Values bigger than INT_MAX should make you feel bad
        case unit_ul:
            return (int)lround( (amount/1000 * 600.0) );
        case unit_pts:
            return (int) lround(amount);
        case unit_mlps:
            return (int) lround( (amount / 600.0) );
        case unit_mlpm:
            return 0;
        case unit_nan:  // TODO: Add method to check inputs of units
        default:
            return 0;
    }
}