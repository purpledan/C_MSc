//
// Created by dan on 08/02/23.
//

#ifndef C_MSC_DCL_DCODE_H
#define C_MSC_DCL_DCODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dcl_config.h"
#include "dcl_msgQueue.h"
#include "dcl_fsm.h"

/* Bitfield defines for dcode status */
#define SCNCMP  0b00000001      // Scan of file complete

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
    state_dcode_EOF,
    state_dcode_abort,
    state_dcode_exit,
    dcode_numstates
}state_dcode;

typedef struct dcode_file {
    char file_name[DCL_DCODE_NAME_LEN];                             // Name of the .dcode input file
    FILE *file_pointer;                                             //
    char line_buf[DCL_DCODE_MAXLINE_LEN];                           // Buffer to hold read line
    char *selector;                                                 // Used to pass index around between states
    size_t line_no;                                                 // Line number to report possible errors to user
    fpos_t step_pos[DCL_DCODE_MAXSTEPS];                            // Positions of steps in file
}dcode_file;

typedef struct dcode_triC_config {
    char valve_names[DCL_TRIC_VALVENO][DCL_DCODE_NAME_LEN];         // User given names for valves
    int valve_speeds[DCL_TRIC_VALVENO];                             // Speed setting for valves
    int default_speed;                                              // Default speed setting see pg 69 of C3000 manual
}dcode_triC_config;

typedef struct dcode_cluster {
    dcl_fsm_cluster_type *fsm;
    dcode_file file;
    dcode_triC_config config;                                       // Future use to pass config via files
    dcode_block block;                                              // Current parsing block
    size_t stepNo;                                                  // Step index
    char steps[DCL_DCODE_MAXSTEPS][DCL_DCODE_NAME_LEN];             // Array which holds user step names
}dcode_cluster;

dcode_cluster *state_dcodeFsm_setup(char *file_name, dcl_queue_type *queue_in);
state_dcode state_dcodeFsm_init(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_scan(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_blkStart(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_blkEnd(dcode_cluster *cluster_in);
state_dcode state_dcodeFsm_config(dcode_cluster *cluster_in);

dcode_block dcode_parse_label(dcode_cluster *cluster_in, char *selection);
#endif //C_MSC_DCL_DCODE_H