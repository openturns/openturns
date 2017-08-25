#
# Compile and create OpenTURNS installer for windows
#
# Rules dependencies make things done in the right order, however it does not
# detect if OpenTURNS sources has changed. This has to be specified manually:
# - to recompute a rules, remove the corresponding file
#   i.e. some OpenTURNS .cxx have changed:
#     rm ot-compile; make
# - to recompute every rules: make clean; make
#

###############################################################################
# adapt these following lines to your configuration :

# compiler prefix
ARCH=i686
TARGET = $(ARCH)-w64-mingw32

# OpenTurns paths
OT_SRC    ?= $(PWD)/../..
OT_BUILD  ?= $(OT_SRC)/build-$(TARGET)
OT_PREFIX ?= $(OT_BUILD)/install

OT_VERSION ?= $(shell cat ../../VERSION)

WINDEPS = $(PWD)/openturns-developers-windeps

# Wine
WINE=/usr/bin/wine
WINE_PREFIX = $(WINDEPS)/wine-$(ARCH)
export WINEPREFIX = $(WINE_PREFIX)
ifeq ($(ARCH), i686)
export WINEARCH = win32
else
export WINEARCH = win64
endif

MINGW_PREFIX = $(WINDEPS)/mingw-w64-x86_64/usr

# Python
PYBASEVER = 2.7
PYBASEVER_NODOT = $(shell echo $(PYBASEVER) | sed "s|\.||g")
PYBASEVER_MAJOR = $(shell echo $(PYBASEVER) | head -c 1)
PYTHON_EXECUTABLE=$(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).exe
export PYTHONHOME := $(MINGW_PREFIX)/$(TARGET)
export PYTHONPATH := $(MINGW_PREFIX)/$(TARGET)/lib/python$(PYBASEVER_NODOT)


R_PATH = $(WINDEPS)/opt/R-2.12.0
PATH := $(R_PATH)/bin:$(PATH)
CMAKE=cmake


# launch OpenTURNS checktests before creating the installer (y,n)
CHECK_OT=y

JOBS=$(shell getconf _NPROCESSORS_ONLN)
JOBS_PYTHON=$(JOBS)
JOBS_TEST=$(JOBS)
JOBS_COTIRE=$(JOBS)
export PATH := $(MINGW_PREFIX)/bin:$(PATH)

DEBUG_OT=n


###############################################################################
# rules:

.PHONY: all clean-python clean mrproper


all: ot-installer


ot-check-compiler:
	@echo -n "Check OpenTURNS dependencies: " 
	@ls $(OT_SRC) $(PYTHON_EXECUTABLE) > /dev/null
	@echo "ok"
	touch ot-check-compiler
ifeq ($(PYBASEVER_MAJOR),3)
	wine wineboot
	xvfb-run winetricks -q vcrun2015
endif

ot-config: ot-check-compiler
	-mkdir -p $(OT_BUILD)
	cp $(MINGW_PREFIX)/share/mingw/toolchain-$(TARGET).cmake $(OT_BUILD)
	echo "set(CMAKE_FIND_ROOT_PATH $(MINGW_PREFIX)/$(TARGET))" >> $(OT_BUILD)/toolchain-$(TARGET).cmake
	cp $(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).dll $(OT_BUILD)
	cd $(OT_BUILD) && rm -f CMakeCache.txt && $(CMAKE) \
	  -DCMAKE_TOOLCHAIN_FILE=toolchain-$(TARGET).cmake \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_C_FLAGS_RELWITHDEBINFO="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4" \
	  -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4 -D_hypot=hypot" \
	  -DPYTHON_INCLUDE_DIR=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
	  -DPYTHON_INCLUDE_DIR2=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
	  -DPYTHON_LIBRARY=$(MINGW_PREFIX)/$(TARGET)/lib/libpython$(PYBASEVER_NODOT).dll.a \
	  -DPYTHON_EXECUTABLE=$(PYTHON_EXECUTABLE) \
	  -DR_EXECUTABLE=$(R_PATH)/bin/R.exe \
	  -DCMAKE_INSTALL_PREFIX=$(OT_PREFIX) \
	  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
	  -DUSE_TBB=OFF \
	  -DUSE_SPHINX=OFF \
	  -DCMAKE_CROSSCOMPILING_EMULATOR=$(WINE) \
	  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j$(JOBS_COTIRE)" \
	  $(OT_SRC)
	touch ot-config

ot-compile: ot-config
	cd $(OT_BUILD) && make OT -j$(JOBS)
	cd $(OT_BUILD) && make -j$(JOBS_PYTHON)
	touch ot-compile

ot-install: ot-compile
	cd $(OT_BUILD) && make install -j$(JOBS)
ifeq ($(DEBUG_OT),n)
	$(TARGET)-strip --strip-unneeded $(OT_PREFIX)/bin/*.dll
	$(TARGET)-strip -g $(OT_PREFIX)/lib/*.a
	$(TARGET)-strip --strip-unneeded $(OT_PREFIX)/Lib/site-packages/*/*.pyd
endif
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgcc_s_*-1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libquadmath-0.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgfortran-*.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libstdc++-6.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libwinpthread-1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libblas.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/liblapack.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libiconv-2.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/liblzma-5.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libxml2-2.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/muparser.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/zlib1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libnlopt-0.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgomp-1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libcblas.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libhmat-oss.dll $(OT_PREFIX)/bin
	tar cjf openturns-mingw-$(OT_VERSION)-py$(PYBASEVER)-$(ARCH).tar.bz2 --directory $(OT_PREFIX)/.. `basename $(OT_PREFIX)`
	touch ot-install

ot-test: ot-install
	cd $(OT_BUILD) && make tests -j$(JOBS)
ifeq ($(CHECK_OT),y)
	cp $(OT_PREFIX)/bin/*.dll $(OT_BUILD)/lib/test
	cp $(OT_PREFIX)/bin/*.dll $(OT_BUILD)/python/test
	cp $(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).dll $(OT_BUILD)/python/test
	cd $(OT_BUILD) && ctest -j$(JOBS_TEST) --output-on-failure --timeout 1500
endif
	touch ot-test

ot-install-test: ot-test
	cp $(OT_SRC)/lib/test/*.expout $(OT_PREFIX)/share/openturns/examples
	for exp_file in `find $(OT_SRC)/python/test -name "t_*.expout"`; do \
	  base_file=`basename $${exp_file} .expout`; \
	  cp $${exp_file} $(OT_PREFIX)/share/openturns/examples/$${base_file}.py.expout; done
	cp $(OT_BUILD)/lib/test/*.exe $(OT_PREFIX)/share/openturns/examples
ifeq ($(DEBUG_OT),n)
	$(TARGET)-strip --strip-all $(OT_PREFIX)/share/openturns/examples/*.exe
endif
	touch ot-install-test

ot-check-installer: ot-install
	@echo -n "Check OpenTURNS prefix: "
	@ls $(OT_PREFIX) > /dev/null
	@echo "ok"
	@echo -n "Check nsis compiler: "
	@makensis -VERSION > /dev/null 2>&1
	@echo "ok"
	@echo "OpenTURNS installer dependencies seems all right."
	touch ot-check-installer

ot-module-installer: ot-check-installer
	sed "s|@version@|$(OT_VERSION)|g" OpenTURNSDoc.url.in > OpenTURNSDoc.url
	makensis -DOPENTURNS_PREFIX=$(OT_PREFIX) -DPRODUCT_VERSION=$(OT_VERSION) -DPYBASEVER=$(PYBASEVER) -DPYBASEVER_NODOT=$(PYBASEVER_NODOT) -DARCH=$(ARCH) openturns.nsi
	touch ot-module-installer

ot-dev-installer: ot-install-test ot-check-installer
	makensis -DOPENTURNS_PREFIX=$(OT_PREFIX) -DPRODUCT_VERSION=$(OT_VERSION) -DPYBASEVER=$(PYBASEVER) -DPYBASEVER_NODOT=$(PYBASEVER_NODOT) -DARCH=$(ARCH) openturns_developer.nsi
	touch ot-dev-installer


ot-installer: ot-module-installer ot-dev-installer
	@echo "ok"	


clean:
	-rm -f ot-*
	-cd $(OT_BUILD) && make clean


mrproper:
	-rm -f ot-*
	-rm -rf $(OT_PREFIX) $(OT_BUILD)
	-rm -f *.exe

