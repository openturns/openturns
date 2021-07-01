#!/bin/sh

set -xe

uid=$1
gid=$2

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
if test -n "${uid}" -a -n "${gid}"
then
  zip -r openturns-doc.zip ~/.local/share/openturns/doc/html/*
  sudo chown ${uid}:${gid} openturns-doc.zip && sudo cp openturns-doc.zip ${source_dir}
fi
ctest -R pyinstallcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
make tests
ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
