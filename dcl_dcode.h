/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 08/02/23.*/

#ifndef C_MSC_DCL_DCODE_H
#define C_MSC_DCL_DCODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "dcl_config.h"
#include "dcl_structs.h"
#include "dcl_fsm.h"

/* Bitfield defines for dcode status */
#define SCNCMP  0b00000001      // Scan of file complete

/*CONST defines */
#define DCODE_ARGNO 4

typedef enum dcode_block {
    block_outside,
    block_config,
    block_step,
    block_run
} dcode_block;

typedef enum state_dcode{
    state_dcode_init,
    state_dcode_scan,
    state_dcode_blkStart,
    state_dcode_blkEnd,
    state_dcode_config,
    state_dcode_step,
    state_dcode_run,
    state_dcode_abort,
    state_dcode_exit,
    dcode_numstates
}state_dcode;

typedef enum dcode_unit {
    unit_ul,        // microliters
    unit_ml,        // milliliters
    unit_pts,       // Location integer units
    unit_mlps,      // milliliters per second
    unit_mlpm,      // milliliters per minute
    unit_pre,       // preset from C3000 manual
    unit_rpm,       // Revolutions per min
    unit_rps,       // Revolutions per sec
    unit_kel,       // Temperature in kelvin
    unit_C,         // Temperature in Celsius
    unit_nan,
}dcode_unit;

typedef struct dcode_file {
    FILE *file_pointer;                                             //
    char line_buf[DCL_DCODE_MAXLINE_LEN];                           // Buffer to hold read line
    char *selector;                                                 // Used to pass index around between states
    size_t line_no;                                                 // Line number to report possible errors to user
    fpos_t step_pos[DCL_DCODE_MAXSTEPS];                            // Positions of steps in file
}dcode_file;

typedef struct dcode_dev_config {
    int dev_num;                                                   // Device Number
    char dev_name[DCL_DCODE_NAME_LEN];                             // User given name for device
    char arg_names[DCL_TRIC_VALVENO][DCL_DCODE_NAME_LEN];          // User given names for arguments
    int arg_values[DCL_TRIC_VALVENO];                              // User given argument values
    int default_arg;
    double default_float;
    struct dcode_dev_config *next_dev;                            // Next Config block
}dcode_dev_config;

typedef struct dcode_dev_steps {
    char step_name[DCL_DCODE_NAME_LEN];                             // Name of current step
    int index;                                                      // Index of current action
    char block[32][DCL_STRMSG_LEN];                                 // action data block TODO: Make dynamic
    struct dcode_dev_steps *next_step;                             // Next step block
}dcode_dev_steps;

typedef struct dcode_cluster {
    dcl_fsm_cluster *fsm;
    dcode_file file;
    dcode_block block;                                              // Current parsing block
    dcode_dev_steps *step_list;                                    // Linked list containing steps
    dcode_dev_steps *current_step;
    dcode_dev_config *config_list;                                       // Future use to pass config via files
    dcode_dev_config *current_config;
}dcode_cluster;

typedef struct dcode_valve {
    int pump_no;
    int valve_no;
    int rate;
    bool valid;
}dcode_valve;

typedef struct dcode_args {
    char *line_in;
    int argc;
    char **argv;
}dcode_args;

dcode_cluster *state_dcodeM_setup(FILE *fp, dcl_queue_type *queue_in);
state_dcode state_dcodeM_init(dcode_cluster *cluster_in);
state_dcode state_dcodeM_scan(dcode_cluster *cluster_in);
state_dcode state_dcodeM_blkStart(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_blkEnd(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_config(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_step(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_run(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_abort(dcode_cluster *cluster_in);

void dcode_rem_wSpace(char* restrict line_out, const char* restrict line_in);
void dcode_rem_comments(char *line_in);
void dcode_step_lexer(dcode_args *args_in);
void dcode_config_lexer(dcode_args *args_in);
void dcode_run_lexer(dcode_args *args_in);
dcode_valve dcode_search_valve(dcode_cluster *cluster_in, char *name);
dcode_unit dcode_search_unit(char *unit_in);
int dcode_unit_convert(dcode_cluster *cluster_in, double amount, dcode_unit unit);
#endif //C_MSC_DCL_DCODE_H