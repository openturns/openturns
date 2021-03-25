#!/bin/sh

set -xe

env

# build with frozen date unless on release for reproducible builds
if test "${CIRCLE_BRANCH}" = "master"
then
  export SOURCE_DATE_EPOCH=1514764800
fi

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
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS" -DSWIG_COMPILE_FLAGS="-O1 -Wno-unused-parameter -Wno-missing-field-initializers" \
      ${source_dir}
make install

