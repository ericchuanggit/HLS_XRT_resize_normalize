#!/bin/bash

if [ -z "$SDKTARGETSYSROOT" ]; then
    echo "sysroot is not set. Please source your SDK environment first."
    exit 1
fi

g++ findrgb.cpp -o output -I$SDKTARGETSYSROOT/usr/include/boost/compute/interop/opencv -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
./output