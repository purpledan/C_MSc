cmake_minimum_required(VERSION 3.24)
project(C_MSc C)

set(CMAKE_C_STANDARD 17)

add_executable(C_MSc main.c DCL_MsgQueue.c DCL_MsgQueue.h TST_MsgQueue.c TST_MsgQueue.h)

# Optimise the release build only, show all build steps when debug
set(CMAKE_C_FLAGS_DEBUG  "-O0 -v -glldb -Wall -Werror -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "-O3")
