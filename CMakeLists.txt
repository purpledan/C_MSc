cmake_minimum_required(VERSION 3.24)
project(C_MSc C)

set(CMAKE_C_STANDARD 17)

add_executable(C_MSc main.c dcl_msgQueue.c dcl_msgQueue.h dcl_serial.c dcl_serial.h dcl_string.c dcl_string.h dcl_triKont.c dcl_triKont.h dcl_fsm_triKont.c dcl_fsm_triKont.h dcl_threads.c dcl_threads.h dcl_config.h dcl_fsm.c dcl_fsm.h dcl_dcode.c dcl_dcode.h)
#add_executable(TST_MsgQueue.c TST_MsgQueue.h)
# Optimise the release build only, show all build steps when debug
set(CMAKE_C_FLAGS_DEBUG  "-O0 -v -Wall -Werror -fno-omit-frame-pointer -fsanitize=address")
#set(CMAKE_C_FLAGS_RELEASE "-O2")
