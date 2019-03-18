#!/bin/sh

set -xe

mkdir build && cd build

ARCH=x86_64
MINGW_PREFIX=/usr/${ARCH}-w64-mingw32
PYMAJMIN=37
PREFIX=${PWD}/install
CXXFLAGS="-Wall -D_GLIBCXX_ASSERTIONS" ${ARCH}-w64-mingw32-cmake \
  -DSWIG_COMPILE_FLAGS="-O0" \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${PREFIX}/lib \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_TBB=OFF \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j16" ..
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${PREFIX}/bin/*.dll ${PREFIX}/Lib/site-packages/openturns/*.pyd
echo lib/test python/test | xargs -n 1 cp ${PREFIX}/bin/*.dll
ctest -R pyinstallcheck --output-on-failure --timeout 1000 ${MAKEFLAGS}
make tests
ctest -R cppcheck --output-on-failure --timeout 1000 ${MAKEFLAGS}
