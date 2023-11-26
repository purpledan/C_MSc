/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 2023/11/23. */

#include "dcl_config.h"

#define MATCH(s, n) ( strcmp(section, s) == 0 && strcmp(name, n) == 0 )
int conf_handler(void *user, const char *section, const char *name, const char *value) {
    dcl_conf *conf_ptr = (dcl_conf*) user;

    if ( MATCH("serialports", "syringepump") ) {
        conf_ptr->addr_SP = strdup(value);
    } else if ( MATCH("serialports", "heater") ) {
        conf_ptr->addr_magStir = strdup(value);
    } else {
        return -1;
    }
    return 1;
}
#undef MATCH

int conf_reader(FILE *cfg_file, ini_handler handler, void *config) {
    int ini_error = ini_parse_file(cfg_file, conf_handler, config);
    switch (ini_error) {
        case 0:
            break;
        case -1:
            fprintf(stderr,"Config.ini failed to open, aborting\n");
            return -1;
        case -2:
            fprintf(stderr, "Config.ini malloc failure, aborting\n");
            return -1;
        default:
            fprintf(stderr, "Error on line: %d, aborting\n", ini_error);
            fclose(cfg_file);
            return -1;
    }
    fclose(cfg_file);
    return 0;
}

int conf_writer(void) {
    FILE *cfg_ini = fopen("config.ini", "w+");
    if (!cfg_ini) {
        perror("Could not write config.ini: ");
        return -1;
    }

    fprintf(cfg_ini, "[serialports]\n");
    fprintf(cfg_ini, "syringepump = %s\n",DCL_CONF_DEFAULT_PUMP);
    fprintf(cfg_ini, "heater = %s\n", DCL_CONF_DEFAULT_MAGSTIR);

    if ( fclose(cfg_ini) == 0) {
        return 0;
    } else {
        perror("Could not close new config.ini");
        return -1;
    }
}


