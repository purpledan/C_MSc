#!/bin/bash
echo "Today is " `date`
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" -S ./ -B ./cmake-build-debug
cmake --build ./cmake-build-debug --target C_MSc -- -j 3