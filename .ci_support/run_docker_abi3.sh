#!/bin/sh

set -xe
git config --global --add safe.directory /io

env

cd /tmp

# current swig 4.4.0 has a bug for abi3
git clone -b v4.4.1 --depth 1 https://github.com/swig/swig.git
cmake -DCMAKE_INSTALL_PREFIX=~/.local -B swig/build swig
make install -C swig/build

cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_LINKER_TYPE=MOLD \
      -DSWIG_COMPILE_FLAGS="-O1 -DPy_LIMITED_API=0x03090000" \
      -DUSE_PYTHON_SABI=ON \
      -DSWIG_EXECUTABLE=~/.local/bin/swig \
      -DBLA_VENDOR=Generic \
      -B build /io
cd build
make install
ctest -R pyinstallcheck --output-on-failure --timeout 200 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
