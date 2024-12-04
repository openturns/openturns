#!/bin/sh

set -xe
git config --global --add safe.directory /io
sudo pacman -S clang libc++ --noconfirm

env

cd /tmp
mkdir build && cd build

# disable all precompiled c++ dependencies for abi compatibility
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
      -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
      -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic -Werror" -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Wshadow -Werror -stdlib=libc++ -D_LIBCPP_ENABLE_ASSERTIONS=1 -Wno-unused-but-set-variable" \
      -DSWIG_COMPILE_FLAGS="-O1 -Wno-shadow -Wno-unused-function" \
      -DUSE_MUPARSER=OFF -DUSE_PRIMESIEVE=OFF -DUSE_TBB=OFF -DUSE_HDF5=OFF \
      -DUSE_PAGMO=OFF  -DUSE_IPOPT=OFF -DUSE_BONMIN=OFF -DUSE_NLOPT=OFF -DUSE_CERES=OFF -DUSE_DLIB=OFF \
      /io
cmake --build . --target install
OPENTURNS_NUM_THREADS=1 ctest -R pyinstallcheck --output-on-failure --timeout 200 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
