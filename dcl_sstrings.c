/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 2023/09/02. */


#include "dcl_sstrings.h"

void dcl_sstr_remWSpace(char *str_in, size_t sstrlen) {

    char *srcp = str_in;
    char *strp = str_in;

    if (!srcp || !strp) {
        return;
    }
    // 01234567 9
    // Potato P\0
    //      *^

    for (size_t i = 0; i < sstrlen; i++) {
        if (!*strp) {
            break;
        };
        if (!isblank(*strp)) {
            *srcp = *strp;
            srcp++;
        }
        strp++;
    }
    *srcp = '\0';
}

void dcl_sstr_remComments(char *str_in, char commentsym, size_t sstrlen) {
    /* strchr will blindly search until a NULL char is reached, lets force one just in case */
    str_in[sstrlen - 1] = '\0';
    char *string_p = strchr(str_in, commentsym);
    if (string_p) {
        *string_p = '\0';
    }
}

char *dcl_sstr_retNum_p(char *str_in, size_t sstrlen) {
    char *strp = str_in;

    if (!strp) {
        return NULL;
    }

    for (size_t i = 0; i < sstrlen; i++) {
        if (!*strp) {
            break;
        }

        if (isdigit(*strp)) {
            return strp;
        }
        strp++;
    }

    return strp;
}

char *dcl_sstr_retWSpace_p(char *str_in, size_t sstrlen) {
    char *strp = str_in;

    if (!strp) {
        return NULL;
    }

    for (size_t i = 0; i < sstrlen; i++) {
        if (!*strp) {
            break;
        }

        if (isspace(*strp)) {
            return strp;
        }
        strp++;
    }

    return strp;
}

int dcl_sstr_strsep(char *argv[], char *str_in, char delimiter, bool useWspace, size_t argvlen, size_t sstrlen) {
    /* TODO: Double check for any overflow shit */
    int index = 0;
    if (!argv || !str_in) {
        return -1;
    }
    argv[index] = str_in;
    index++;

    char *strp = str_in;
    for (size_t i = 0; i < sstrlen; i ++) {
        if (!*strp) { /* Reaching end of string breaks out of loop */
            break;
        }
        if (*strp == delimiter || ( useWspace && isspace(*strp) ) ) {
            *strp = '\0';
            strp++;
            argv[index] = strp;
            index++;
        }
        if (index >= argvlen) { /* Don't exceed the number of args allowed */
            break;
        }
        strp++;
    }


    return index + 1; /* We want the number of args, not just the index */
}