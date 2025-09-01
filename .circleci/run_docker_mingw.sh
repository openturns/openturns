#!/bin/sh

set -xe
git config --global --add safe.directory /io

if test -d "/io"
then
  source_dir=/io  # local
  cd /tmp
else
  source_dir=`pwd`  # circleci
fi

mkdir build && cd build

ARCH=x86_64
MINGW_PREFIX=/usr/${ARCH}-w64-mingw32
PYMAJMIN=310
PREFIX=${PWD}/install
CFLAGS="-Wall -Wextra -Wpedantic -Werror" \
CXXFLAGS="-Wall -Wextra -Wpedantic -Wshadow -Wsuggest-override -Werror -D_GLIBCXX_ASSERTIONS" \
${ARCH}-w64-mingw32-cmake \
  -DSWIG_COMPILE_FLAGS="-O1 -Wno-unused-parameter -Wno-shadow" -DSWIG_CXX_COMPILE_FLAGS="-Wno-suggest-override" \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${PREFIX}/lib \
  -DCMAKE_LINKER_TYPE=LLD \
  -DBLA_VENDOR=Generic \
  -DPython_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPython_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPython_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  ${source_dir}
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${PREFIX}/bin/*.dll ${PREFIX}/Lib/site-packages/openturns/*.pyd
echo lib/test ${PREFIX}/Lib/site-packages/openturns | xargs -n 1 cp ${PREFIX}/bin/*.dll
ctest -R pyinstallcheck --output-on-failure ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
make tests
ctest -R cppcheck --output-on-failure --timeout 1000 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
