#!/bin/bash

echo -e "1. linux\n2. mingw\n\n> "
read choice
case $choice in
  1)
    docker pull openturns/archlinux-python
    docker run -e MAKEFLAGS='-j8' -v `pwd`:/io openturns/archlinux-python /io/.circleci/run_docker_linux.sh
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
