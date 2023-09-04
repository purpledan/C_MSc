/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by dan on 2023/09/02. */

/*  Files: dcl_sstrings.h & dcl_sstrings.c
 *  Library to handle statically allocated C strings
 *
 *  Manipulates strings in place
 *
 */

#ifndef C_MSC_DCL_SSTRINGS_H
#define C_MSC_DCL_SSTRINGS_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

/* Removes white space from a string resulting in a shrunken string */
void dcl_sstr_remWSpace(char *str_in, size_t sstrlen);

/* Removes comments from a string, i.e. any chars after the comment symbol */
void dcl_sstr_remComments(char *str_in, char commentsym, size_t sstrlen);

/* Searches for numeric value and returs pointer to first digit */
char *dcl_sstr_retNum_p(char *str_in, size_t sstrlen);

/* Searches for numeric value, returns pointer to first digit, and eliminates trailing garbage */
char *dcl_sstr_retNumClean_p(char *str_in, size_t sstrlen);

/* Separates delimited string into an array of pointers to strings, option to specify delimiters and wSpace separation */
int dcl_sstr_strsep_pp(char *argv[], char *str_in, char delimiter, bool useWspace, size_t argvlen, size_t sstrlen);


#endif //C_MSC_DCL_SSTRINGS_H
