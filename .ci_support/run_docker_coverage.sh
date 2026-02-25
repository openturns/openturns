#!/bin/sh

set -xe
git config --global --add safe.directory /io

UID_GID=$1

env

cd /tmp
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_LINKER_TYPE=MOLD \
      -DCMAKE_C_FLAGS="--coverage" -DCMAKE_CXX_FLAGS="--coverage" \
      -DSWIG_COMPILE_FLAGS="-O1" \
      -DBLA_VENDOR=Generic \
      -DUSE_HMAT=ON \
      -B build /io
cd build
make install
OPENTURNS_NUM_THREADS=1 ctest -R pyinstallcheck --output-on-failure --timeout 200 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
#make tests
#OPENTURNS_NUM_THREADS=2 ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random

# coverage
gcov `find lib/src/ -name "*.gcno"`
time lcov --capture --directory . --output-file coverage.info --include "*.cxx" --exclude "*_wrap.cxx" -j 4
genhtml --output-directory coverage coverage.info
cp -v coverage.info coverage

if test -n "${UID_GID}"
then
  sudo chown -R ${UID_GID} coverage && sudo cp -r coverage /io
fi
