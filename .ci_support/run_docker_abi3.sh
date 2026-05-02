#!/bin/sh

set -xe
git config --global --add safe.directory /io

env

cd /tmp

cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Werror -D_GLIBCXX_ASSERTIONS" \
      -DCMAKE_LINKER_TYPE=MOLD \
      -DSWIG_COMPILE_FLAGS="-O1 -DPy_LIMITED_API=0x03090000" \
      -DUSE_PYTHON_SABI=ON \
      -B build /io
cd build
make install
ctest -R pyinstallcheck --output-on-failure --timeout 200 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
