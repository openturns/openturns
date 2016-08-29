Windows port
============

This documentation aims at guiding the developer with cross-compilation for
the Windows target using MinGW-w64 project.
This is the method used to ship the windows binaries that
include Python bindings.

Library compilation
-------------------

The cross-compilation is straightforward thanks to the provided precompiled
MinGW toolchain and libraries:

1. Install Wine, NSIS::

    aptitude install wine nsis

2. Fetch the last openturns-developers-windeps-x.y.tgz archive and unzip it::

    cd openturns-src/distro/windows
    wget http://downloads.sourceforge.net/openturns/openturns/openturns-1.8/dev/openturns-developers-windeps-1.8.tar.gz
    tar xzf openturns-developers-windeps-1.8.tar.gz

3. Launch compilation::

    make
    make ARCH=x86_64  # (for a 64 bits target)
    make PYBASEVER=3.5  # (for Python 3.5)

If you are lucky, it should produce an installer named openturns-x.y-pyu.v-arch.exe


Module compilation
------------------

A successful build of the library according to the previous step is required to
compile a module from its sources::

    cd module-src/distro/windows
    make OT_PREFIX=$PWD/../../../openturns-src/build-i686-w64-mingw32/install


The same Makefile variables can be set (the same way the library was built)
to control the build parameters.
If the build is successful an executable installer is created.
