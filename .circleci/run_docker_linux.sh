#!/bin/sh

set -xe
git config --global --add safe.directory /io

UID_GID=$1

env

# build with frozen date unless on release for reproducible builds
if test "${CIRCLE_BRANCH}" = "master"
then
  export SOURCE_DATE_EPOCH=1640995200
fi

if test -d "/io"
then
  source_dir=/io  # local
  cd /tmp
else
  source_dir=`pwd`  # circleci
fi

mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=${HOME}/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic -Werror" \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Wshadow -Wsuggest-override -Werror -D_GLIBCXX_ASSERTIONS" \
      -DCMAKE_LINKER_TYPE=MOLD \
      -DBLA_VENDOR=Generic \
      -DSWIG_COMPILE_FLAGS="-O1 -Wno-unused-parameter -Wno-shadow" -DSWIG_CXX_COMPILE_FLAGS="-Wno-suggest-override" \
      -DUSE_SPHINX=ON -DSPHINX_FLAGS="-W -T -j4" \
      ${source_dir}
make install
find ~/.local/share/doc/openturns/html -name "*.zip" | xargs rm
if test -n "${UID_GID}"
then
  cp -r ~/.local/share/doc/openturns/html .
  zip -r openturns-doc.zip html/*
  sudo chown ${UID_GID} openturns-doc.zip && sudo cp -p openturns-doc.zip ${source_dir}
fi
ctest -R pyinstallcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
#make tests
#ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
