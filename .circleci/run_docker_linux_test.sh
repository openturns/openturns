#!/bin/sh

set -xe

uid=$1
gid=$2

env

## copy build variables
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
## end copy build variables

cd build

if test -n "${uid}" -a -n "${gid}"
then
  zip -r openturns-doc.zip ~/.local/share/openturns/doc/html/*
  sudo chown ${uid}:${gid} openturns-doc.zip && sudo cp openturns-doc.zip ${source_dir}
fi
ctest -R pyinstallcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
make tests
ctest -R cppcheck --output-on-failure --timeout 100 ${MAKEFLAGS} --repeat after-timeout:2 --schedule-random
