#!/bin/bash

PATH_SHARED_LIB="/usr/lib/"
PATH_HEADER="/usr/include/"

echo "Install to :"
echo "  ${SDKTARGETSYSROOT}/${PATH_SHARED_LIB}"

# install to SDK if using cross compile
cp _hls_resize_normalize.pc "${SDKTARGETSYSROOT}/${PATH_SHARED_LIB}/pkgconfig"
cp _build/lib_hls_resize_normalize.so "${SDKTARGETSYSROOT}/${PATH_SHARED_LIB}"
cp inc/hls_resize_normalize.hpp "${SDKTARGETSYSROOT}/${PATH_HEADER}"