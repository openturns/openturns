Windows port
============

This documentation aims at guiding the developer with cross-compilation for
the Windows target using MinGW-w64 project.
This is the method used to ship the windows binaries that
include Python bindings.

Library compilation
-------------------

The cross-compilation is performed using a custom Docker image providing
precompiled MinGW toolchain and libraries::

    git clone https://github.com/openturns/build.git
    cd build
    docker pull openturns/archlinux-mingw
    docker run -e MAKEFLAGS='-j2' -v `pwd`:/io openturns/archlinux-mingw /io/run_docker_build.sh 1.12 3.7 x86_64 `id -u` `id -g`

If you are lucky, it should produce an installer named openturns-x.y-pyu.v-arch.exe
