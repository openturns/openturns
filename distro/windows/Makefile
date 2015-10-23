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

WINDEPS = $(PWD)/openturns-developers-windeps-$(OT_VERSION)

# Wine
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
PYTHON_EXECUTABLE=$(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).exe
export PYTHONHOME := $(MINGW_PREFIX)/$(TARGET)
export PYTHONPATH := $(MINGW_PREFIX)/$(TARGET)/lib/python$(PYBASEVER_NODOT)


R_PATH = $(WINDEPS)/opt/R-2.12.0
PATH := $(R_PATH)/bin:$(PATH)
CMAKE=cmake


# launch OpenTURNS checktests before creating the installer (y,n)
CHECK_OT=y

# build and install tests
INSTALL_TESTS_OPT=ON

JOBS=$(shell getconf _NPROCESSORS_ONLN)
JOBS_PYTHON=$(JOBS)
JOBS_TEST=$(JOBS)
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


ot-config: ot-check-compiler
	-mkdir -p $(OT_BUILD)
	cp $(MINGW_PREFIX)/share/mingw/toolchain-$(TARGET).cmake $(OT_BUILD)
	echo "set(CMAKE_FIND_ROOT_PATH $(MINGW_PREFIX)/$(TARGET))" >> $(OT_BUILD)/toolchain-$(TARGET).cmake
	cd $(OT_BUILD) && rm -f CMakeCache.txt && $(CMAKE) \
	  -DCMAKE_TOOLCHAIN_FILE=toolchain-$(TARGET).cmake \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_C_FLAGS_RELWITHDEBINFO="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4" \
	  -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4" \
	  -DPYTHON_INCLUDE_DIR=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
	  -DPYTHON_INCLUDE_DIR2=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
	  -DPYTHON_LIBRARY=$(MINGW_PREFIX)/$(TARGET)/lib/libpython$(PYBASEVER_NODOT).dll.a \
	  -DPYTHON_EXECUTABLE=$(PYTHON_EXECUTABLE) \
	  -DR_EXECUTABLE=$(R_PATH)/bin/R.exe \
	  -DCMAKE_INSTALL_PREFIX=$(OT_PREFIX) \
	  -DINSTALL_TESTS=$(INSTALL_TESTS_OPT) \
	  -DUSE_TBB=OFF \
	  -DUSE_SPHINX=OFF \
	  $(OT_SRC)
	touch ot-config

ot-compile: ot-config
	cd $(OT_BUILD)/lib && make -j$(JOBS)
	cd $(OT_BUILD) && make -j$(JOBS_PYTHON)
	-beep
	touch ot-compile

ot-install: ot-compile
	cd $(OT_BUILD) && make install -j$(JOBS)
ifeq ($(DEBUG_OT),n)
	$(TARGET)-strip --strip-unneeded $(OT_PREFIX)/bin/*.dll
	$(TARGET)-strip -g $(OT_PREFIX)/lib/*.a
	$(TARGET)-strip --strip-unneeded $(OT_PREFIX)/Lib/site-packages/*/*.pyd
ifeq ($(INSTALL_TESTS_OPT),ON)
	$(TARGET)-strip --strip-all $(OT_PREFIX)/share/openturns/examples/*.exe
endif
endif
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgcc_s_*-1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libquadmath-0.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgfortran-3.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libstdc++-6.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libwinpthread-1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libblas.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/liblapack.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libiconv-2.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libxml2-2.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libdl*.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgnurx-0.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/*muparser*.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/zlib1.dll $(OT_PREFIX)/bin
# 	cp $(MINGW_PREFIX)/$(TARGET)/bin/libtbb.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgomp-1.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libcblas.dll $(OT_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libhmat-oss.dll $(OT_PREFIX)/bin
	-beep -r 2
	touch ot-install

ot-test: ot-install
	cd $(OT_BUILD) && make tests -j$(JOBS)
ifeq ($(CHECK_OT),y)
	cp $(OT_PREFIX)/bin/*.dll $(OT_BUILD)/lib/test
	cp $(OT_PREFIX)/bin/*.dll $(OT_BUILD)/python/test
	cd $(OT_BUILD) && ctest -j$(JOBS_TEST) --timeout 1500
	touch ot-test
endif

ot-check-installer: ot-test
	@echo -n "Check OpenTURNS prefix: "
	@ls $(OT_PREFIX) > /dev/null
	@echo "ok"
	@echo -n "Check nsis compiler: "
	@makensis -VERSION > /dev/null 2>&1
	@echo "ok"
	@echo "OpenTURNS installer dependencies seems all right."
	touch ot-check-installer

ot-module-installer: ot-check-installer
	makensis -DOPENTURNS_PREFIX=$(OT_PREFIX) -DPRODUCT_VERSION=$(OT_VERSION) -DPYBASEVER=$(PYBASEVER) -DPYBASEVER_NODOT=$(PYBASEVER_NODOT) -DARCH=$(ARCH) openturns.nsi
	touch ot-module-installer


ot-dev-installer: ot-check-installer
	makensis -DOPENTURNS_PREFIX=$(OT_PREFIX) -DPRODUCT_VERSION=$(OT_VERSION) -DPYBASEVER=$(PYBASEVER) -DPYBASEVER_NODOT=$(PYBASEVER_NODOT) -DARCH=$(ARCH) openturns_developer.nsi
	touch ot-dev-installer


ot-installer: ot-module-installer ot-dev-installer
	-beep -r 3


clean:
	-rm -f ot-*
	-cd $(OT_BUILD) && make clean


mrproper:
	-rm -f ot-*
	-rm -rf $(OT_PREFIX) $(OT_BUILD)
	-rm -f *.exe


VCBOTTLE = $(PWD)/bottle-vc10
OT_BUILD_VC ?= $(OT_SRC)/build-vc

ot-vc:
	-mkdir -p $(OT_BUILD_VC)
	cd $(OT_BUILD_VC) && rm -f CMakeCache.txt && $(VCBOTTLE)/vc-cmake \
	  -DBUILD_PYTHON=OFF \
	  -DUSE_DOXYGEN=OFF \
	  -DUSE_BISON=OFF \
	  -DUSE_FLEX=OFF \
	  -DUSE_TBB=OFF \
	  -DUSE_HMAT=OFF \
	  -DUSE_MUPARSER=OFF \
	  -DUSE_LIBXML2=OFF \
	  -DUSE_R=OFF \
	  -DUSE_NLOPT=OFF \
	  -DBOOST_ROOT=\"C:\\local\\\\boost_1_58_0\" \
	  -DCMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING=\"/OPT:NOREF /INCREMENTAL:NO\" \
	  -DLAPACKE_FOUND=TRUE \
	  -DOPENTURNS_LIBRARIES=\"C:\\local\\OpenBLAS-v0.2.14-Win32\\lib\\libopenblas.dll.a\" \
	  -Dot_configure_date=\"now\" \
	$(OT_SRC)
	cd $(OT_BUILD_VC) && $(VCBOTTLE)/vc-make OT -j$(JOBS)
	cd $(OT_BUILD_VC) && $(VCBOTTLE)/vc-make t_NumericalPoint_std t_ComparisonOperator_std t_Interval_std t_RandomGenerator_std t_Combinations_std t_Mesh_std t_Arcsine_std
	cp $(OT_BUILD_VC)/lib/src/OT.dll $(OT_BUILD_VC)/lib/test
	cp $(VCBOTTLE)/drive_c/local/OpenBLAS-v0.2.14-Win32/bin/libopenblas.dll $(OT_BUILD_VC)/lib/test
	cp $(MINGW_PREFIX)/i686-w64-mingw32/bin/libquadmath-0.dll $(OT_BUILD_VC)/lib/test
	cp $(MINGW_PREFIX)/i686-w64-mingw32/bin/libgcc_s_sjlj-1.dll $(OT_BUILD_VC)/lib/test
	cp $(MINGW_PREFIX)/i686-w64-mingw32/bin/libwinpthread-1.dll $(OT_BUILD_VC)/lib/test
	cp $(MINGW_PREFIX)/i686-w64-mingw32/bin/libgfortran-3.dll $(OT_BUILD_VC)/lib/test
	for testname in `find $(OT_BUILD_VC)/lib/test -name "*.exe"|sed "s|.*/t_\(.*\)\.exe|\1|"`; do \
	  cd $(OT_BUILD_VC)/lib/test && echo "Test: $$testname" && OPENTURNS_CONFIG_PATH=$(OT_BUILD_VC)/lib/etc ./t_$$testname.exe | tr -d '\015' > t_$$testname.out && diff -u $(OT_SRC)/lib/test/t_$$testname.expout t_$$testname.out && echo "Test Passed" || echo "Test Failed"; \
	done
	touch ot-vc
