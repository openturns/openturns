#!/bin/sh

apt-get -y update && apt-get -y install git g++ python3-matplotlib libxml2-dev liblapack-dev libnlopt-cxx-dev libdlib-dev libceres-dev libmpfr-dev libmpc-dev libgoogle-glog-dev libgflags-dev libprimesieve-dev cmake swig python3-dev libcminpack-dev libboost-math-dev libmetis-dev libeigen3-dev libpng-dev libsqlite3-dev libjpeg-dev libpagmo-dev libnanoflann-dev libpython3-dev coinor-libipopt-dev coinor-libbonmin-dev libhdf5-dev libspectra-dev python3-pandas

set -e
git config --global --add safe.directory /io

cd /tmp
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DSWIG_COMPILE_FLAGS="-O1" -B build /io
cd build
make install
OPENTURNS_NUM_THREADS=1 OMP_NUM_THREADS=1 ctest -R pyinstall --output-on-failure --timeout 1000 ${MAKEFLAGS} --schedule-random
