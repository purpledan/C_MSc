/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 31/01/23.*/

#ifndef C_MSC_DCL_CONFIG_H
#define C_MSC_DCL_CONFIG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inih/ini.h"

/* Static Global defines */
/* Message string length for queue based FSM communication */
#define DCL_STRMSG_LEN 64
#define DCL_DCODE_NAME_LEN 16
#define DCL_DCODE_MAXSTEPS 32
#define DCL_DCODE_MAXLINE_LEN 128
#define DCL_DCODE_ARGNUM 6
#define DCL_DCODE_ARGLEN 12
#define DCL_TRIC_VALVENO 6
#define DCL_TRIC_PUMPNO 2
#define DCL_TRIC_FD ""

/* Config.ini handling */

#define DCL_CONF_DEFAULT_PUMP "/dev/ttyUSB0"
#define DCL_CONF_DEFAULT_MAGSTIR "/dev/ttyUSB1"

/* This struct hold all the pointers to strings, inih has a static limit of 200 chars per line */
typedef struct {
    char *addr_SP;              // Serial port address for syringe pump/s
    char *addr_magStir;         // Serial port address for Magnetic stirrer
} dcl_conf;

/* The following is based on the example given by github.com/benhoyt/inih */
int conf_handler(void *user, const char *section, const char *name, const char *value);

/* Wrapper for ini_parse_file */
int conf_reader(FILE *cfg_file, ini_handler handler, void *config);

/* Write a default config file, does not leave FILE* open*/
int conf_writer(void);

#endif //C_MSC_DCL_CONFIG_H
