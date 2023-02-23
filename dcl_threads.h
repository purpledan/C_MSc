//
// Created by Daniel on 2023/01/23.
//

#ifndef C_MSC_DCL_THREADS_H
#define C_MSC_DCL_THREADS_H

#include <pthread.h>
#include "dcl_structs.h"
#include "dcl_triKont.h"
#include "dcl_serial.h"
#include "dcl_fsm_triKont.h"
#include "dcl_dcode.h"

void *pumpThread(void *arg);

void *parserThread(void *arg);

#endif //C_MSC_DCL_THREADS_H
