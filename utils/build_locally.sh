#!/bin/bash

if test "$#" -lt 1
then
  echo -e "1. sphinx\n2. mingw\n3. coverage\n4. aarch64\n5. mindeps\n\n> "
  read choice
else
  choice="$1"
fi

case $choice in
  "1" | "sphinx")
    docker pull openturns/archlinux-python
    docker run --rm -e MAKEFLAGS -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/archlinux-python /io/.circleci/run_docker_linux.sh `id -u`:`id -g`
    ;;
  "2" | "mingw")
    docker pull openturns/archlinux-mingw
    docker run --rm -e MAKEFLAGS -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/archlinux-mingw /io/.circleci/run_docker_mingw.sh
    ;;
  "3" | "coverage")
    docker pull openturns/archlinux-python
    docker run --rm -e MAKEFLAGS -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/archlinux-python /io/.ci_support/run_docker_coverage.sh `id -u`:`id -g`
    ;;
  "4" | "aarch64")
    docker pull openturns/debian-aarch64
    docker run --rm -e MAKEFLAGS -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/debian-aarch64 /io/.ci_support/run_docker_aarch64.sh
    ;;
  "5" | "mindeps")
    docker pull debian:11
    docker run --rm -e MAKEFLAGS -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io debian:11 /io/.ci_support/run_docker_mindeps.sh
    ;;
  *)
    echo "sorry?"
    exit 1
    ;;
esac
