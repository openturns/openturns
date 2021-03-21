#!/bin/bash

echo -e "1. linux\n2. mingw\n\n> "
read choice
case $choice in
  1)
    docker pull openturns/archlinux-python
    docker run -e MAKEFLAGS='-j8' -v `pwd`:/io openturns/archlinux-python /io/.circleci/run_docker_linux.sh `id -u` `id -g`
    docker run -e MAKEFLAGS='-j8' -v `pwd`:/io openturns/archlinux-python /io/.circleci/run_docker_linux_test.sh `id -u` `id -g`
    ;;
  2)
    docker pull openturns/archlinux-mingw
    docker run -e MAKEFLAGS='-j8' -v `pwd`:/io openturns/archlinux-mingw /io/.circleci/run_docker_mingw.sh
    ;;
  *)
    echo "sorry?"
    exit 1
    ;;
esac
