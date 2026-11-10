#!/bin/sh

set -e
git config --global --add safe.directory /io

cd /tmp
cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -D_GLIBCXX_ASSERTIONS" -DCMAKE_INSTALL_PREFIX=$PWD/install -DSWIG_COMPILE_FLAGS="-O1" -S /io -B build
cd build
make install
OPENTURNS_NUM_THREADS=1 ctest -R pyinstallcheck --output-on-failure --timeout 200 --schedule-random ${MAKEFLAGS}

