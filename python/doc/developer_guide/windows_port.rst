Windows port
============

This documentation aims at guiding the developer with cross-compilation for
the Windows target.
This documentation is separated into two main parts :

-  compile OpenTURNS under Linux for Windows target,

-  validation and use of OpenTURNS on Windows.

Linux side
----------

Quick compilation guide
~~~~~~~~~~~~~~~~~~~~~~~

cross-compilation is now quite straightforward:

-  install Wine and NSIS (e.g. ``aptitude install wine nsis``)

-  | fetch the last openturns-developers-windeps-x.y.tgz corresponding
     to the x.y version on
     http://sourceforge.net/projects/openturns/files/openturns/openturns-x.y,
     untar it in distro/windows/ folder:
   | ``cd openturns-src/distro/windows ; tar zxf openturns-developers-windeps-x.y.tgz``

   Note: the given MinGW compiler is build for a x86\_64 Linux host.

-  | launch compilation:
   | ``cd openturns-src/distro/windows ; make``

-  | or launch compilation for a 64 bits target (the default is i686):
   | ``cd openturns-src/distro/windows ; make ARCH=x86_64``

-  | or launch compilation for another python version (the default is
     2.7):
   | ``cd openturns-src/distro/windows ; make PYBASEVER=3.5``

-  | That’s it. If you are lucky, it should produce 2 installers (.exe).


Linux test Environment
^^^^^^^^^^^^^^^^^^^^^^

WINE
''''

Install any `WINE <http://www.winehq.org/>`_ version, for example, the one
given by your Linux distribution.

Windows shared libraries
''''''''''''''''''''''''

-  put the shared libraries of pthreads (pthreadGC2.dll), BLAS/LAPACK
   (lapack.dll and blas.dll), dlfcn (libdl.dll), libxml2
   (libcharset-1.dll libiconv-2.dll libxml2-2.dll zlib1.dll), and regex
   (libgnurx-0.dll) in a directory where the PATH environment variable
   of WINE points to.

-  put the shared library given by MinGW (mingwm10.dll) in this
   directory too.

Note : WINE’s PATH can be modified in the file ~/.wine/system.reg.

Advice
''''''

It is better to install OpenTURNS dependencies in directory without
spaces (e.g. not in *C:\\Program Files*). The space between *Program*
and *Files* can cause cumbersome problems.

Ghostscript
'''''''''''

-  download and install ghostscript into WINE environment. The installer
   (e.g. gs864w32.exe) can be found here
   http://sourceforge.net/projects/ghostscript/. Launch the command like
   this : wine gs864w32.exe.

-  add the path to gswin32c.exe to the PATH environment variable of
   WINE.

R
'

-  install R into WINE environment by using the standard Windows
   installer from the `official site <http://cran.r-project.org>`_.

-  add the path to R.exe to the PATH environment variable of WINE.

R packages
''''''''''

-  install the and rotR zip files with Rgui.exe (menu packages =>
   install R packages fom zip files).

The package rot\_1.4.5.tar.gz have been transformed to Windows packages
with the website http://win-builder.r-project.org.

Python
''''''

-  download the 2.7 version of Windows python installer from the
   official site http://www.python.org/download/. Install python using
   this command : wine msiexec /i python-2.7.3.msi .

-  add the path to Python.exe to the PATH environment variable of WINE.

matplotlib module
'''''''''''''''''

Install a matplotlib version compatible with Python2.7 (e.g.:
matplotlib-1.2.1.win32-py2.7.exe) from matplotlib
`download <http://matplotlib.org/downloads.html>`_ section.

Compilation
^^^^^^^^^^^

In order to cross-compile:

First get the type of your computer in order to set the ``--build``
configure settings :

::

    export BUILD_MACHINE=`gcc -dumpmachine`

The configuration step:

::

    # adapt these following lines to your configuration:
    ARCH=i686
    TARGET = $(ARCH)-w64-mingw32

    # OpenTurns paths
    OT_SRC    ?= $(PWD)/../..
    OT_BUILD  ?= $(OT_SRC)/build-$(TARGET)
    OT_PREFIX ?= $(OT_BUILD)/install

    PYBASEVER=2.7
    PYTHON_PREFIX=$HOME/.wine/drive_c/Python$PYTHON_VERSION
    R_PATH=$HOME/.wine/drive_c/R/R-2.9.0
    MINGW_PREFIX=/opt/mingw32

    # Python
    PYBASEVER = 2.7
    PYBASEVER_NODOT = $(shell echo $(PYBASEVER) | sed "s|\.||g")
    PYTHON_EXECUTABLE=$(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).exe
    export PYTHONHOME := $(MINGW_PREFIX)/$(TARGET)
    export PYTHONPATH := $(MINGW_PREFIX)/$(TARGET)/lib/python$(PYBASEVER_NODOT)

    # launch as is :
    $(TARGET)-cmake \
              -DCMAKE_TOOLCHAIN_FILE=toolchain-$(TARGET).cmake \
              -DCMAKE_VERBOSE_MAKEFILE=$(VERBOSE) \
              -DPYTHON_INCLUDE_DIR=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
              -DPYTHON_LIBRARY=$(MINGW_PREFIX)/$(TARGET)/lib/libpython$(PYBASEVER_NODOT).dll.a \
              -DPYTHON_EXECUTABLE=$(PYTHON_EXECUTABLE) \
              -DR_EXECUTABLE=$(R_PATH)/bin/R.exe \
              -DCMAKE_INSTALL_PREFIX=$(OT_PREFIX) \
              -DINSTALL_TESTS=$(INSTALL_TESTS_OPT) \
              $(OT_SRC)

Debug symbols are stripped so that binaries are 3 times smaller:

::

    $(TARGET)-strip --strip-unneeded $(OT_PREFIX)/bin/*.dll
    $(TARGET)-strip -g $(OT_PREFIX)/lib/*.a
    $(TARGET)-strip --strip-unneeded $(OT_PREFIX)/Lib/site-packages/*/*.pyd

In the same shell, start the compilation :

::

    # openturns compilation and installation
    make; make install

The validation : launch the following command :

::

    # set the PATH to python.exe
    PATH=$PATH:$PYTHON_PREFIX

    make check && make installcheck

How to create the installer
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Two installers are created using NSIS.

-  openturns-x.y-pyu.v-arch.exe installs the DLL and headers, and its
   dependencies. It is targeted users that interact through Python.

-  openturns-developers-x.y-arch.exe helps launching the tests.

Windows side
------------

Install OpenTURNS manually
~~~~~~~~~~~~~~~~~~~~~~~~~~

To install OpenTURNS without installer (the following points are done
automatically by the installer openturns-x.y-pyu.v-arch.exe) :

-  Copy the *install* directory (created by the command make install)
   from Linux to Windows into the directory *C:\\openturns*.

-  Like with WINE, every DLL must be reachable (mingwm, pthread,
   BLAS/LAPACK, dlfcn, libxml2, regex and OpenTurns), and the programs
   must be installed : R with its packages, ghostscript, Python with the
   required modules.

   On Windows, DLLs are searched in directories listed in the PATH
   environment variable. To set the PATH variable temporarily, hit on a
   DOS console :

   ::

       set PATH=%PATH%;C:\openturns\bin;C:\openturns\lib\bin
       echo %PATH%

   To set permanently the PATH variable : configuration panel -> system
   -> tab “advanced” -> button “environment variable” -> list “system
   variable” -> modify PATH variable.

Install with a non-admin account
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use installer as usual.

compilation examples
~~~~~~~~~~~~~~~~~~~~

Visual C++
^^^^^^^^^^

The ABI of C++ binaries produced by Visual C++ and g++ are not
compatible (C ABI are compatible). ABI means Application Binary
Interface. Further informations can be found here :
http://chadaustin.me/cppinterface.html.

-  So if you need to link your program compiled with Visual C++ with
   OpenTURNS DLL, it is not possible. But if you need to use only a
   small subset of the OpenTURNS C++ interface, one can use a workaround
   and make an had-hoc MinGW wrapper that wrap OpenTURNS C++ symbols to
   C symbols (C binaries are compatible between gcc and Visual C). The
   application compiled with Visual Studio will be able to interact with
   OpenTURNS through the C symbols of the wrapper. The following diagram
   explains this:

   ::

       prog vc++
       |
       ABI C
       |
       hadhoc wrapper g++
       |
       ABI g++
       |
       OpenTURNS g++

Benchmark
^^^^^^^^^

No official benchmark of OpenTURNS on Windows has been done, but windows
version is slower than Linux one.

Resolved problems
-----------------

-  if DLLs or programs are not found :

   check your MSYS or Windows PATH environment variable.

-  if OpenTURNS does not start from python interpreter and if the
   PYTHONPATH is correctly set :

   check that the version of the python interpreter is the same as the
   version OpenTURNS has been compiled for.

-  if a program is installed in C:\\Program Files and if it is not well
   detected,

   reinstall it in directory without spaces in the name. The space
   between *Program* and *Files* can cause cumbersome problems.

-  to modify the PATH variable of .wine/system.reg, no WINE process must
   be started. When a WINE process stops, it overwrites this files.
