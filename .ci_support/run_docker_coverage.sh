#!/bin/sh

set -xe

uid=$1
gid=$2

env

if test -d "/io"
then
  source_dir=/io  # local
  cd /tmp
else
  source_dir=`pwd`  # circleci
fi

mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_C_FLAGS="--coverage" -DCMAKE_CXX_FLAGS="--coverage -fuse-ld=mold" \
      -DSWIG_COMPILE_FLAGS="-O1" \
      -DUSE_SPHINX=OFF \
      ${source_dir}
make install
OPENTURNS_NUM_THREADS=1 ctest -R pyinstallcheck --output-on-failure --timeout 200 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
#make tests
#OPENTURNS_NUM_THREADS=2 ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random

# coverage
gcov `find lib/src/ -name "*.gcno"`
lcov --capture --directory . --output-file coverage.info --include "*.cxx"
genhtml --output-directory coverage coverage.info
cp -v coverage.info coverage

if test -n "${uid}" -a -n "${gid}"
then
  sudo chown -R ${uid}:${gid} coverage && sudo cp -r coverage ${source_dir}
fi
