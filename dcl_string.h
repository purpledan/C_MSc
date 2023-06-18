/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/
/* Created by Daniel on 2022/08/23.*/

#ifndef INSTRUMENTMAN_DCL_STRING_H
#define INSTRUMENTMAN_DCL_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** String Lib for use in sending ASCII data to instruments
 * Static size strings: Usually used in sending data
 * Dynamic size strings: Used in receiving data from an instrument or sending large data streams
 *
 * Static string functions are rude and unsafe.
 * They should only be used internally and never allow a buffer to be filled by a user or file.
 */

#define SEP_CHARS 8
#define STATIC_SIZE 128
#define DYNAMIC_SIZE 64
#define DYNAMIC_GROW 32
#define DYNAMIC_MAX 2048

typedef struct dcl_string_box {
    char box_name[STATIC_SIZE];         // Helpful box name
    size_t maxRetCnt;                   // Maximum allowed return arguments
    char* stagingBuf;                   // Main string storage
    size_t buf_size;                    // Storage available
    size_t buf_len;                     // Storage used
    char* sep_chars[SEP_CHARS];     // Valid Separators
    size_t retCount;                    // Return strings count
    char** retStrings;                  // Pointers to strings in storage
}dcl_string_box;

int dcl_string_box_open(dcl_string_box* box_in);
int dcl_string_box_insert(dcl_string_box* box_in, char* string_in, size_t in_len);
int dcl_string_box_strsep(dcl_string_box* box_in, char sepChar, char termChar);
int dcl_string_box_print(dcl_string_box* box_in);
int dcl_string_box_empty(dcl_string_box* box_in);
void dcl_string_box_close(dcl_string_box* box_in);

int dcl_string_box_grow(dcl_string_box* box_in, size_t grw_len);



#endif //INSTRUMENTMAN_DCL_STRING_H
