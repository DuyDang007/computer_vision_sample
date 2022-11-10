#!/bin/bash

WORK=`pwd`

EXECUTABLE=${WORK}/build/sfm_test.exe

# PATH to linked libs
PATH=${PATH}:${WORK}/../opencv4.5.5/x64/mingw/bin

${EXECUTABLE}