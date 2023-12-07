#!/bin/bash

make clean

VITIS_LIBRARY=${1:-/home/eric/Vitis_Libraries}

if ! [ -d "${VITIS_LIBRARY}" ]; then
    echo "Vitis_Libraries not exist, download from git and modify this script ${0}"
    exit
fi

export OPENCV_INCLUDE=/usr/local/include/opencv4
export OPENCV_LIB=/usr/local/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

make run CSIM=1 CSYNTH=0 COSIM=0 XPART="xck26-sfvc784-2LVI-i" XF_PROJ_ROOT="${VITIS_LIBRARY}/vision"