#!/bin/sh
apt-get -y update && apt-get -y install git g++ python3-matplotlib libxml2-dev liblapack-dev cmake swig python3-dev libcminpack-dev libboost-math-dev

set -e

cd /tmp
cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -D_GLIBCXX_ASSERTIONS" -DCMAKE_INSTALL_PREFIX=$PWD/install -DSWIG_COMPILE_FLAGS="-O1" -S /io -B build
cd build
make install
OPENTURNS_NUM_THREADS=1 ctest -R pyinstallcheck --output-on-failure --schedule-random ${MAKEFLAGS} -E FunctionalChaosSobolIndices_std

