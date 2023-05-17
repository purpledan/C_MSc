/*******************************************************************************
 * Copyright (c) 2023. Daniel Janse van Rensburg - All Rights Reserved
 * Dan's Control Library (DCL) is subject to future license changes.
 ******************************************************************************/

//
// Created by dan on 17/05/23.
//

#include "dcl_time.h"

void dcl_printTime(void) {
    char paint[12] = "";
    struct tm cur_time = {
            .tm_sec = 0,
    };
    time_t t;
    t = time(NULL);
    localtime_r(&t, &cur_time);
    strftime(paint, 12, "%T", &cur_time);
    printf("(%s) ", paint);
}
