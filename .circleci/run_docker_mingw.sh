#!/bin/sh

set -xe

if test -d "/io"
then
  source_dir=/io  # local
  cd /tmp
else
  source_dir=`pwd`  # circleci
fi

mkdir build && cd build

ARCH=i686
MINGW_PREFIX=/usr/${ARCH}-w64-mingw32
PYMAJMIN=310
PREFIX=${PWD}/install
CXXFLAGS="-Wall -Wextra -Wpedantic -Wshadow -Werror -D_GLIBCXX_ASSERTIONS" ${ARCH}-w64-mingw32-cmake \
  -DSWIG_COMPILE_FLAGS="-O0 -Wno-unused-parameter -Wno-shadow" \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${PREFIX}/lib \
  -DPython_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPython_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPython_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DUSE_TBB=OFF \
  ${source_dir}
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${PREFIX}/bin/*.dll ${PREFIX}/Lib/site-packages/openturns/*.pyd
echo lib/test ${PREFIX}/Lib/site-packages/openturns | xargs -n 1 cp ${PREFIX}/bin/*.dll
OPENTURNS_NUM_THREADS=2 ctest -R pyinstallcheck --output-on-failure ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
make tests
OPENTURNS_NUM_THREADS=2 ctest -R cppcheck --output-on-failure --timeout 1000 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
