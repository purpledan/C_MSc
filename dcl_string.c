//
// Created by Daniel on 2022/08/23.
//

#include "dcl_string.h"

int dcl_string_box_open(dcl_string_box* box_in) {
    box_in->buf_size = DYNAMIC_SIZE;
    box_in->stagingBuf = malloc(DYNAMIC_SIZE);
    if (!box_in->stagingBuf) {
        fprintf(stderr, "Malloc Failed for: %s", box_in->box_name);
        perror("");
        return -1;
    }
    box_in->retStrings = malloc(box_in->maxRetCnt * sizeof(char*) );
    if (!box_in->retStrings) {
        fprintf(stderr, "Malloc Failed for: %s", box_in->box_name);
        perror("");
        return -1;
    }
    dcl_string_box_empty(box_in);
    return 0;
}

int dcl_string_box_insert(dcl_string_box* box_in, char* string_in, size_t in_len) {

    if ( in_len >= (box_in->buf_size - box_in->buf_len) ) {
        if ( dcl_string_box_grow(box_in, ( in_len - box_in->buf_size) + DYNAMIC_GROW) < 0 ) {
            return -1;
        }
    }
    strcat(box_in->stagingBuf, string_in);
    box_in->buf_len += in_len;
    return 0;
}

int dcl_string_box_strsep(dcl_string_box* box_in, char sepChar, char termChar) {
    for (size_t i = 0; i < box_in->buf_len; i++) {
        if ( box_in->stagingBuf[i] == sepChar || box_in->stagingBuf[i] == termChar) {
            box_in->stagingBuf[i] = '\0';
            box_in->retCount++;
        }
    }

    size_t ret_iterator = 0;
    box_in->retStrings[0] = box_in->stagingBuf;
    for (size_t i = 0; i < box_in->buf_len; i++) {
        if (box_in->stagingBuf[i] == '\0') {
            ret_iterator++;
            box_in->retStrings[ret_iterator] = &box_in->stagingBuf[i + 1];
        }
    }

    return 0;
}

int dcl_string_box_print(dcl_string_box* box_in) {
    for (size_t i = 0; i < box_in->retCount; i++) {
        printf("Part %zu: %s\n", i,box_in->retStrings[i]);
    }
    return 0;
}

int dcl_string_box_empty(dcl_string_box* box_in) {
    box_in->stagingBuf[0] = '\0';
    box_in->buf_len = 0;
    box_in->retCount = 0;
    return 0;
}

int dcl_string_box_grow(dcl_string_box* box_in, size_t grw_len) {
    char* tmp_pointer = box_in->stagingBuf;

    if ( (box_in->buf_size + grw_len) >= DYNAMIC_MAX ) {
        fprintf(stderr, "Max memory limit reached for: %s\n", box_in->box_name);
        return -1;
    }
    box_in->stagingBuf = realloc(box_in->stagingBuf, box_in->buf_size + grw_len);
    if (!box_in->stagingBuf) {
        fprintf(stderr, "Realloc Failed for: %s", box_in->box_name);
        perror("");
        box_in->stagingBuf = tmp_pointer;
        return -1;
    }

    box_in->buf_size += grw_len;
    return 0;
}

void dcl_string_box_close(dcl_string_box* box_in) {
    free(box_in->retStrings);
    free(box_in->stagingBuf);
    box_in->stagingBuf = NULL;
    box_in->retStrings = NULL;
}