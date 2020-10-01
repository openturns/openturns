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
PYMAJMIN=38
PREFIX=${PWD}/install
CXXFLAGS="-Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS" ${ARCH}-w64-mingw32-cmake \
  -DSWIG_COMPILE_FLAGS="-O0 -Wno-unused-parameter -Wno-missing-field-initializers" \
  -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${PREFIX}/lib \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_TBB=OFF \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DUSE_HDF5=OFF \
  ${source_dir}
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${PREFIX}/bin/*.dll ${PREFIX}/Lib/site-packages/openturns/*.pyd
echo lib/test ${PREFIX}/Lib/site-packages/openturns | xargs -n 1 cp ${PREFIX}/bin/*.dll
ctest -R pyinstallcheck --output-on-failure ${MAKEFLAGS}
make tests
ctest -R cppcheck --output-on-failure --timeout 1000 ${MAKEFLAGS}
