#!/bin/sh

set -xe

# build with frozen date unless on release for reproducible builds
if test "${CIRCLE_BRANCH}" = "master"
then
  export SOURCE_DATE_EPOCH=1514764800
fi

mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j16" \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS" -DSWIG_COMPILE_FLAGS="-O1 -Wno-error=unused-parameter" \
      ..
make install
ctest -R pyinstallcheck --output-on-failure --timeout 100 ${MAKEFLAGS}
make tests
ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS}
