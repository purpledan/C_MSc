cmake_minimum_required(VERSION 3.24)
project(C_MSc C)

set(CMAKE_C_STANDARD 17)

add_executable(C_MSc main.c dcl_structs.c dcl_structs.h dcl_serial.c dcl_serial.h dcl_string.c dcl_string.h
        dcl_triKont.c dcl_triKont.h dcl_fsm_triKont.c dcl_fsm_triKont.h dcl_threads.c dcl_threads.h
        dcl_config.h dcl_config.c dcl_fsm.c dcl_fsm.h dcl_dcode.c dcl_dcode.h dcl_time.c dcl_time.h dcl_heat.c
        dcl_heat.h dcl_sstrings.c dcl_sstrings.h dcl_fsm_heat.c dcl_fsm_heat.h dcl_supervisor.c dcl_supervisor.h
        inih/ini.c inih/ini.h)

#add_executable(TST_MsgQueue.c TST_MsgQueue.h)
# Optimise the release build only, show all build steps when debug
set(CMAKE_C_FLAGS_DEBUG  "-O0 -ggdb -v -lm -Wall -Werror -fno-omit-frame-pointer")
#set(CMAKE_C_FLAGS_RELEASE "-O2")
