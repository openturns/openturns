#!/bin/sh
# build & test

set -e

R CMD INSTALL --library=$PWD utils/rot_1.4.6.tar.gz
export R_LIBS=$PWD
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DPYTHON_EXECUTABLE=/usr/bin/python3 \
      -DPYTHON_INCLUDE_DIR=/usr/include/python3.4m \
      -DPYTHON_INCLUDE_DIR2=/usr/include/python3.4m \
      -DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.4m.so \
      -DSPHINX_EXECUTABLE=~/.local/bin/sphinx-build \
      -DSWIG_EXECUTABLE=~/.local/bin/swig \
      -DHMAT_DIR=~/.local/lib/cmake/hmat \
      -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j2" \
      -DCMAKE_CXX_FLAGS="-Wall -D_GLIBCXX_ASSERTIONS" \
      .
make install -j2
ctest -R pyinstallcheck --output-on-failure --timeout 100 -j2
