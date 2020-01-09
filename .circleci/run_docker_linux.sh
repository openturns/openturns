#!/bin/sh

set -xe

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

cd /usr/lib/python3.8/site-packages/matplotlib/ && sudo patch -p3 -i ${source_dir}/mpl.patch && cd -

mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS" -DSWIG_COMPILE_FLAGS="-O1 -Wno-unused-parameter -Wno-missing-field-initializers" \
      -DSPHINX_FLAGS="-W -T -j2" \
      ${source_dir}
make install
ctest -R pyinstallcheck --output-on-failure --timeout 100 ${MAKEFLAGS}
make tests
ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS}
