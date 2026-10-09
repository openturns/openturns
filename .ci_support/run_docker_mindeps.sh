#!/bin/sh

printf '%s\n' \
  'deb http://snapshot.debian.org/archive/debian/20260701T000000Z bullseye main' \
  'deb http://snapshot.debian.org/archive/debian/20260701T000000Z bullseye-updates main' \
  'deb http://snapshot.debian.org/archive/debian-security/20260701T000000Z bullseye-security main' > /etc/apt/sources.list
printf 'Acquire::Check-Valid-Until "false";\n' > /etc/apt/apt.conf.d/99snapshot

apt-get -y update && apt-get -y install git g++ python3-matplotlib libxml2-dev liblapack-dev cmake swig python3-dev libcminpack-dev libboost-math-dev

set -e
git config --global --add safe.directory /io

cd /tmp
cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -D_GLIBCXX_ASSERTIONS" -DCMAKE_INSTALL_PREFIX=$PWD/install -DSWIG_COMPILE_FLAGS="-O1" -S /io -B build
cd build
make install
OPENTURNS_NUM_THREADS=1 ctest -R pyinstallcheck --output-on-failure --timeout 200 --schedule-random ${MAKEFLAGS}

