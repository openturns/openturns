#!/bin/sh
# build & test

set -e

R CMD INSTALL --library=$PWD utils/rot_1.4.6.tar.gz
export R_LIBS=$PWD
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DSPHINX_EXECUTABLE=~/.local/bin/sphinx-build \
      -DSWIG_EXECUTABLE=~/.local/bin/swig \
      -DHMAT_DIR=~/.local/lib/cmake/hmat .
make install -j2
make tests -j2
ctest --output-on-failure --timeout 100
