#!/bin/bash
BUILD_DIR="_build"
rm -rf "${BUILD_DIR}"
rm meson.build
sed -E 's@<SYSROOT>@'"$SDKTARGETSYSROOT"'@g; s@<NATIVESYSROOT>@'"$OECORE_NATIVE_SYSROOT"'@g' meson.build.template > meson.build
meson "${BUILD_DIR}"
cd "${BUILD_DIR}"
ninja --verbose
