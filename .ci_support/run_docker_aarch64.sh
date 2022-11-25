#!/bin/sh

set -e

cd /tmp
mkdir build && cd build
aarch64-cmake -DCMAKE_INSTALL_PREFIX=~/.local \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DPython_EXECUTABLE=/usr/bin/aarch64-python -DSWIG_COMPILE_FLAGS="-O1" /io
make install
ctest -R pyinstall --output-on-failure --timeout 1000 ${MAKEFLAGS} --schedule-random -E ANCOVA
