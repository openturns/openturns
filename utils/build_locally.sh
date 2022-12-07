#!/bin/bash

echo -e "1. linux\n2. mingw\n3. aarch64\n\n> "
read choice
case $choice in
  1)
    docker pull openturns/archlinux-python
    docker run --rm -e MAKEFLAGS='-j8' -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/archlinux-python /io/.circleci/run_docker_linux.sh `id -u` `id -g`
    ;;
  2)
    docker pull openturns/archlinux-mingw
    docker run --rm -e MAKEFLAGS='-j8' -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/archlinux-mingw /io/.circleci/run_docker_mingw.sh
    ;;
  3)
    docker pull openturns/debian-aarch64
    docker run --rm -e MAKEFLAGS='-j8' -e OPENTURNS_NUM_THREADS=2 -v `pwd`:/io openturns/debian-aarch64 /io/.ci_support/run_docker_aarch64.sh
    ;;
  *)
    echo "sorry?"
    exit 1
    ;;
esac
