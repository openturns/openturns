#                                               -*- cmake -*-

cmake_minimum_required (VERSION 3.18)

# By default, build in Release mode. Must appear before project() command
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" Release Debug RelWithDebInfo MinSizeRel)
set (CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_BUILD_TYPE})

project (openturns)

option (USE_CUBA                     "Use Cuba library"                                                      ON)
option (USE_TBB                      "Use Intel Threading Building Blocks library for multithreading"        ON)
option (USE_HMAT                     "Use HMat library (enforces GPL license)"                               OFF)
option (USE_MUPARSER                 "Use muParser library"                                                  ON)
option (USE_EXPRTK                   "Use ExprTK library"                                                    ON)
option (USE_LIBXML2                  "Use LibXml2 for XML support"                                           ON)
option (USE_HDF5                     "Use HDF5 for high volume storage"                                      ON)
option (USE_BOOST                    "Use Boost for distribution computation"                                ON)
option (USE_MPFR                     "Use MPFR for real valued special functions computation"                ON)
option (USE_MPC                      "Use MPC for complex valued special functions computation"              ON)
option (USE_SPHINX                   "Use sphinx for documentation"                                          OFF)
option (USE_DOXYGEN                  "Use Doxygen for API documentation"                                     ON)
option (USE_NLOPT                    "Use NLopt for additional optimization algorithms"                      ON)
option (USE_CERES                    "Use Ceres Solver for additional optimization algorithms"               ON)
option (USE_CMINPACK                 "Use CMinpack for additional optimization algorithms"                   ON)
option (USE_DLIB                     "Use dlib for additional optimization algorithms"                       ON)
option (USE_IPOPT                    "Use Ipopt for nonlinear optimization"                                  ON)
option (USE_BONMIN                   "Use Bonmin for MINLP problems"                                         ON)
option (USE_PAGMO                    "Use Pagmo for multi-objective optimization"                            ON)
option (USE_SPECTRA                  "Use Spectra for eigenvalues computation"                               ON)
option (USE_PRIMESIEVE               "Use primesieve for prime numbers generation"                           ON)
option (USE_OPENMP                   "Use OpenMP to disable threading"                                       ON)
option (USE_OPENBLAS                 "Use OpenBLAS to disable threading"                                     ON)
option (USE_NANOFLANN                "Use nanoflann for Nearest Neighbor search"                             ON)

option (BUILD_PYTHON                 "Build the python module for the library"                               ON)
option (BUILD_SHARED_LIBS            "Build shared libraries"                                                ON)

# Defines our own module path
list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (POLICY CMP0022)
  # INTERFACE_LINK_LIBRARIES defines the link interface
  cmake_policy (SET CMP0022 NEW)
endif()

if (POLICY CMP0057)
  # Support new IN_LIST if() operator
  cmake_policy (SET CMP0057 NEW)
endif ()

if (POLICY CMP0068)
  # RPATH settings on macOS do not affect install_name
  cmake_policy (SET CMP0068 NEW)
endif ()

if (POLICY CMP0093)
  # FindBoost reports Boost_VERSION in x.y.z format
  cmake_policy (SET CMP0093 NEW)
endif ()

if (POLICY CMP0058)
  # Ninja requires custom command byproducts to be explicit
  cmake_policy (SET CMP0058 NEW)
endif()

if(POLICY CMP0042)
  # Set MACOSX_RPATH to ON
  cmake_policy(SET CMP0042 NEW)
endif()

if (POLICY CMP0054)
  cmake_policy (SET CMP0054 NEW)
endif ()

if (POLICY CMP0056)
  cmake_policy (SET CMP0056 NEW)
endif ()

if (POLICY CMP0066)
  cmake_policy (SET CMP0066 NEW)
endif ()

if (POLICY CMP0074)
  # find_package() uses <PackageName>_ROOT variables
  cmake_policy(SET CMP0074 NEW)
endif ()

if (POLICY CMP0078)
  # swig standard target names
  cmake_policy(SET CMP0078 NEW)
endif ()

if (POLICY CMP0086)
  # UseSWIG honors SWIG_MODULE_NAME via -module flag
  cmake_policy(SET CMP0086 NEW)
endif ()

set (OPENTURNS_HOME_ENV_VAR OPENTURNS_HOME)

include (GNUInstallDirs)

set (OPENTURNS_LIBRARY_PATH ${CMAKE_INSTALL_LIBDIR} CACHE PATH "The directory where the library is installed")
set (OPENTURNS_BIN_PATH ${CMAKE_INSTALL_BINDIR} CACHE PATH "Binary install path")
set (OPENTURNS_INCLUDE_PATH ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "The directory where the header files are installed")
set (OPENTURNS_CONFIG_CMAKE_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/openturns CACHE PATH "The directory where the CMake files are installed")
set (OPENTURNS_SYSCONFIG_PATH ${CMAKE_INSTALL_SYSCONFDIR} CACHE PATH "The directory where the configuration file is installed")
set (OPENTURNS_DATA_PATH ${CMAKE_INSTALL_DATAROOTDIR} CACHE PATH "The directory where the common files are installed")
set (OPENTURNS_DOC_PATH ${CMAKE_INSTALL_DOCDIR} CACHE PATH "The directory where the license files are installed")

if (WIN32)
  set (DEFAULT_TMP TEMP)
  set (PATH_SEP "\\;")
else ()
  set(DEFAULT_TMP /tmp)
  set (PATH_SEP ":")
endif()

set (OPENTURNS_TEMPDIR
      ${DEFAULT_TMP}
      CACHE PATH "The directory for temporary files. /tmp by default."
   )
set (TEMPDIR ${OPENTURNS_TEMPDIR})

if (NOT DEFINED LAPACK_LIBRARIES)
  find_package (LAPACK REQUIRED)
endif ()
list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${LAPACK_LIBRARIES})

if (USE_SPECTRA)
  find_package (Spectra 1.0 CONFIG)
endif ()
if (Spectra_FOUND)
  message(STATUS "Found Spectra: ${Spectra_DIR} (found suitable version \"${Spectra_VERSION}\")")
  set (OPENTURNS_HAVE_SPECTRA TRUE)
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES Spectra::Spectra)
  list (APPEND OPENTURNS_ENABLED_FEATURES "spectra")
  if (NOT TARGET Eigen3::Eigen)
    find_package (Eigen3 CONFIG)
  endif ()
endif()

if (USE_PRIMESIEVE)
  find_package (primesieve CONFIG)
  if (primesieve_FOUND)
    set (OPENTURNS_HAVE_PRIMESIEVE TRUE)
    message (STATUS "Found primesieve: ${primesieve_DIR} (found version ${primesieve_VERSION})")
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES primesieve::primesieve)
    list (APPEND OPENTURNS_ENABLED_FEATURES "primesieve")
  else ()
    # fallback to our macro if primesieveConfig is not provided
    find_package (primesieve)
    if (primesieve_FOUND)
      set (OPENTURNS_HAVE_PRIMESIEVE TRUE)
      list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${PRIMESIEVE_INCLUDE_DIRS})
      list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${PRIMESIEVE_LIBRARIES})
      list (APPEND OPENTURNS_ENABLED_FEATURES "primesieve")
    endif ()
  endif ()
endif ()

if (USE_TBB)
  find_package (TBB CONFIG)
  if (TBB_FOUND)
    message (STATUS "Found TBB: ${TBB_DIR} (found version ${TBB_VERSION})")
    set (TBB_LIBRARIES TBB::tbb)
  else ()
    # fallback to our macro
    find_package (TBB 2017)
  endif ()
endif ()
if (TBB_FOUND)
  set (OPENTURNS_HAVE_TBB TRUE)
  if (TBB_INCLUDE_DIRS)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
  endif ()
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${TBB_LIBRARIES})
  if (MSVC OR WINTEL)
    set (OPENTURNS_TBB_NO_IMPLICIT_LINKAGE ON CACHE BOOL "Prevent implicit linkage against tbb.lib ")
  endif ()
  list (APPEND OPENTURNS_ENABLED_FEATURES "tbb")
endif ()

if (USE_MUPARSER)
  find_package (muParser 2.2.3 QUIET)
endif ()
if (MUPARSER_FOUND)
  set (OPENTURNS_HAVE_MUPARSER TRUE)
  set (OPENTURNS_HAVE_ANALYTICAL_PARSER TRUE)
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${MUPARSER_LIBRARIES})
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${MUPARSER_INCLUDE_DIRS})
  list (APPEND OPENTURNS_ENABLED_FEATURES "muparser")
  set (SYMBOLICPARSER_DEFAULT_BACKEND MuParser)
endif ()
if (USE_EXPRTK)
  set (OPENTURNS_HAVE_EXPRTK TRUE)
  set (OPENTURNS_HAVE_ANALYTICAL_PARSER TRUE)
  set (SYMBOLICPARSER_DEFAULT_BACKEND ExprTk)
endif ()
if (NOT OPENTURNS_HAVE_ANALYTICAL_PARSER)
  message(FATAL_ERROR "Analytical parser required, please enable either USE_MUPARSER or USE_EXPRTK")
endif ()

if (USE_HMAT)
  find_package (HMAT 1.7 CONFIG)
endif ()
if (HMAT_FOUND)
  message(STATUS "Found HMAT: ${HMAT_DIR} (found suitable version \"${HMAT_VERSION}\")")
  set (OPENTURNS_HAVE_HMAT TRUE)
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${HMAT_INCLUDE_DIRS})
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${HMAT_LIBRARIES})
  list (APPEND OPENTURNS_ENABLED_FEATURES "hmat")
endif ()

if (USE_DOXYGEN)
  find_package (Doxygen QUIET)
endif ()

if (USE_LIBXML2)
  find_package (LibXml2)
endif ()
if (LibXml2_FOUND)
  set (OPENTURNS_HAVE_LIBXML2 TRUE)
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES LibXml2::LibXml2)
  list (APPEND OPENTURNS_ENABLED_FEATURES "libxml2")
endif ()

if (USE_HDF5)
  find_package(HDF5 COMPONENTS C CXX)
endif ()
if (HDF5_FOUND)
  set (OPENTURNS_HAVE_HDF5 TRUE)
  list (APPEND OPENTURNS_DEFINITIONS ${HDF5_DEFINITIONS})
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS})
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${HDF5_LIBRARIES})
  list (APPEND OPENTURNS_ENABLED_FEATURES "hdf5")
endif ()

if (USE_BOOST)
  find_package (Boost CONFIG 1.70)
  if (Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_DIR} (found suitable version \"${Boost_VERSION}\")")
  endif ()

  # fallback to FindBoost before its deprecation
  if (CMAKE_VERSION VERSION_LESS 3.30 AND NOT Boost_FOUND)
    find_package (Boost MODULE 1.70)
  endif ()

  if (Boost_FOUND)
    set (OPENTURNS_HAVE_BOOST TRUE)
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES Boost::boost)
    list (APPEND OPENTURNS_ENABLED_FEATURES "boost")
    if (USE_MPC)
      find_package (MPC)
      if (MPC_FOUND)
        set (OPENTURNS_HAVE_MPC TRUE)
        list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${MPC_INCLUDE_DIRS})
        list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${MPC_LIBRARIES})
        list (APPEND OPENTURNS_ENABLED_FEATURES "mpc")
      endif ()
    endif ()
    if (USE_MPFR)
      find_package (MPFR)
      if (MPFR_FOUND)
        set (OPENTURNS_HAVE_MPFR TRUE)
        list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${MPFR_INCLUDE_DIRS})
        list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${MPFR_LIBRARIES})
        list (APPEND OPENTURNS_ENABLED_FEATURES "mpfr")
      endif ()
    endif ()
  endif ()
endif ()

if (USE_CUBA)
  find_package (Cuba)
  if (Cuba_FOUND)
    set (OPENTURNS_HAVE_CUBA TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${CUBA_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${CUBA_LIBRARIES})
    list (APPEND OPENTURNS_ENABLED_FEATURES "cuba")
  endif ()
endif ()

if (USE_NLOPT)
  find_package (NLopt 2.6 CONFIG)
  if (NLopt_FOUND)
    message(STATUS "Found NLopt: ${NLopt_DIR}  (found version \"${NLopt_VERSION}\")")
    set (OPENTURNS_HAVE_NLOPT TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${NLOPT_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${NLOPT_LIBRARIES})
    list (APPEND OPENTURNS_ENABLED_FEATURES "nlopt")
  endif ()
endif ()

if (USE_DLIB)
  find_package (dlib 19.8 CONFIG)
  if (dlib_FOUND)
    set (OPENTURNS_HAVE_DLIB TRUE)
    message (STATUS "Found dlib: ${dlib_DIR} (found version ${dlib_VERSION})")
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES dlib::dlib)
    list (APPEND OPENTURNS_ENABLED_FEATURES "dlib")
  endif ()
endif ()

if (USE_CERES)
  find_package (Ceres CONFIG)
  if (CERES_USES_MINIGLOG)
    message (STATUS "Ceres was built with miniglog and will likely leak logs to stderr (prefer glog), disabling.")
    set (Ceres_FOUND FALSE)
  endif ()
  if (Ceres_FOUND)
    set (OPENTURNS_HAVE_CERES TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${CERES_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${CERES_LIBRARIES})
    list (APPEND OPENTURNS_ENABLED_FEATURES "ceres")
  else ()
    # https://github.com/ceres-solver/ceres-solver/issues/1024
    list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
  endif ()
endif ()

if (USE_CMINPACK)
  find_package (CMinpack CONFIG QUIET)
  if (CMinpack_FOUND)
    if (TARGET cminpack::cminpack)
      message (STATUS "Found CMinpack: ${CMinpack_DIR}")
      set (CMINPACK_LIBRARIES cminpack::cminpack)
    endif ()
  else ()
    # fallback to our macro if CMinpackConfig is not provided
    find_package (CMinpack)
    set (CMinpack_FOUND ${CMINPACK_FOUND})
  endif ()
  if (CMinpack_FOUND)
    set (OPENTURNS_HAVE_CMINPACK TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${CMINPACK_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${CMINPACK_LIBRARIES})
    list (APPEND OPENTURNS_ENABLED_FEATURES "cminpack")
  endif ()
endif ()

if (USE_BONMIN)
  find_package (Bonmin)
  if (Bonmin_FOUND)
    set (OPENTURNS_HAVE_BONMIN TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${COIN_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${COIN_BONMIN_LIBRARIES})
    list (APPEND OPENTURNS_ENABLED_FEATURES "bonmin")
  endif ()
endif ()

if (USE_IPOPT)
  find_package (Ipopt)
  if (Ipopt_FOUND)
    set (OPENTURNS_HAVE_IPOPT TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${IPOPT_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${IPOPT_LIBRARIES})
    list (APPEND OPENTURNS_ENABLED_FEATURES "ipopt")
  endif ()
endif ()

if (USE_PAGMO)
  find_package (Pagmo CONFIG)
  if (Pagmo_FOUND)
    message (STATUS "Found Pagmo: ${Pagmo_DIR} (found version \"${Pagmo_VERSION}\")")
    set (OPENTURNS_HAVE_PAGMO TRUE)
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES Pagmo::pagmo)
    list (APPEND OPENTURNS_ENABLED_FEATURES "pagmo")
  endif ()
endif ()

if (USE_OPENMP)
  find_package (OpenMP)
  if (OPENMP_FOUND)
    set (OPENTURNS_HAVE_OPENMP TRUE)
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES OpenMP::OpenMP_CXX)
  endif ()
endif ()

if (USE_NANOFLANN)
  find_package (nanoflann CONFIG)
  if (nanoflann_FOUND)
    message (STATUS "Found nanoflann: ${nanoflann_DIR} (found version \"${nanoflann_VERSION}\")")
    set (OPENTURNS_HAVE_NANOFLANN TRUE)
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES nanoflann::nanoflann)
    list (APPEND OPENTURNS_ENABLED_FEATURES "nanoflann")
  endif ()
endif ()

if (MSVC)
  # Disable some warnings
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4197 /wd4244 /wd4251 /wd4267 /wd4275 /wd4996")
  # for exprtk, boost, swig
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif ()

if (CMAKE_VERSION VERSION_LESS 3.20)
  include (TestBigEndian)
  test_big_endian (DSFMT_BIG_ENDIAN)
else ()
  if (CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
    set (DSFMT_BIG_ENDIAN 1)
  endif ()
endif ()

try_compile(OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/unsignedlong_same_as_uint64_t.cxx)

# on armel atomic must be explicitely linked
if (UNIX AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  try_compile(OPENTURNS_HAVE_INLINE_ATOMIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/have_inline_atomic.cxx)
  message(STATUS "Performing Test OPENTURNS_HAVE_INLINE_ATOMIC - ${OPENTURNS_HAVE_INLINE_ATOMIC}")
  if (NOT OPENTURNS_HAVE_INLINE_ATOMIC)
    find_library (ATOMIC_LIBRARY NAMES atomic)
    if (ATOMIC_LIBRARY)
      list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${ATOMIC_LIBRARY})
    endif ()
  endif ()
endif ()

# for consistent floating-point operations across architectures
option (DISABLE_FP_CONTRACT "Disable floating-point contractions" ON)
if (DISABLE_FP_CONTRACT AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffp-contract=off")
endif ()

# Find system prerequisites
include (CheckIncludeFile)
check_include_file (dlfcn.h       OPENTURNS_HAVE_DLFCN_H      )

include (CheckFunctionExists)
if (OPENTURNS_HAVE_DLFCN_H AND UNIX)
  find_library (LIBDL_LIBRARIES NAMES dl)
  mark_as_advanced (LIBDL_LIBRARIES)
  if (LIBDL_LIBRARIES)
    set (CMAKE_REQUIRED_LIBRARIES ${LIBDL_LIBRARIES})
    check_function_exists (dladdr OPENTURNS_HAVE_DLADDR)
    set (CMAKE_REQUIRED_LIBRARIES)
    if (OPENTURNS_HAVE_DLADDR)
      list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${LIBDL_LIBRARIES})
    endif ()
  endif ()
endif ()

if (USE_OPENBLAS)
  set (CMAKE_REQUIRED_LIBRARIES ${LAPACK_LIBRARIES})
  check_function_exists("openblas_set_num_threads" OPENTURNS_HAVE_OPENBLAS_SET_NUM_THREADS)
  check_function_exists("goto_get_num_procs" OPENTURNS_HAVE_GOTO_GET_NUM_PROCS)
  if (OPENTURNS_HAVE_OPENBLAS_SET_NUM_THREADS AND OPENTURNS_HAVE_GOTO_GET_NUM_PROCS)
    set (OPENTURNS_HAVE_OPENBLAS TRUE)
  endif ()
  set (CMAKE_REQUIRED_LIBRARIES)
endif ()

# Some useful macros to ease CMakeLists.txt file writing
set (SOURCEFILES "" CACHE INTERNAL "List of source files to compile")
macro (ot_add_source_file FILENAME)
  set (sf ${SOURCEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME})
  set (SOURCEFILES ${sf} CACHE INTERNAL "List of source files to compile")
endmacro (ot_add_source_file)

set (HEADERFILES "" CACHE INTERNAL "List of header files to install")
macro (ot_install_header_file FILENAME)
  set (hf ${HEADERFILES} ${CMAKE_CURRENT_SOURCE_DIR}/openturns/${FILENAME})
  set (HEADERFILES ${hf} CACHE INTERNAL "List of header files to install")
endmacro (ot_install_header_file)

set (SWIGFILES "" CACHE INTERNAL "List of SWIG files to install")
macro (ot_install_swig_file FILENAME)
  set (hf ${SWIGFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME})
  set (SWIGFILES ${hf} CACHE INTERNAL "List of SWIG files to install")
endmacro (ot_install_swig_file)

set (INTERNAL_INCLUDE_DIRS "" CACHE INTERNAL "List of directories with header files needed for build")
macro (ot_add_current_dir_to_include_dirs)
  set (inc_dirs ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
  set (INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build")
endmacro (ot_add_current_dir_to_include_dirs)
macro (ot_add_build_dir_to_include_dirs DIR)
  set (inc_dirs ${INTERNAL_INCLUDE_DIRS} ${DIR})
  set (INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build")
endmacro (ot_add_build_dir_to_include_dirs)

if (MSVC OR WINTEL)
  list (APPEND OPENTURNS_DEFINITIONS -D_USE_MATH_DEFINES)
endif ()

if (OPENTURNS_INCLUDE_DIRS)
  list (REMOVE_DUPLICATES OPENTURNS_INCLUDE_DIRS)
endif (OPENTURNS_INCLUDE_DIRS)
include_directories (${OPENTURNS_INCLUDE_DIRS})
include_directories (${OPENTURNS_PRIVATE_INCLUDE_DIRS})

set (CPACK_PACKAGE_NAME          openturns     )
set (CPACK_PACKAGE_VERSION_MAJOR 1             )
set (CPACK_PACKAGE_VERSION_MINOR 26dev)
set (CPACK_PACKAGE_VERSION_PATCH               )
set (CPACK_SOURCE_GENERATOR      "TGZ;TBZ2"    )
set (CPACK_BINARY_STGZ    OFF CACHE BOOL "STGZ")
set (CPACK_BINARY_TBZ2    ON  CACHE BOOL "TBZ2")
set (CPACK_BINARY_TGZ     ON  CACHE BOOL "TGZ" )
set (CPACK_BINARY_TZ      OFF CACHE BOOL "TZ"  )
set (CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;${CPACK_SOURCE_IGNORE_FILES}")


# Set global definitions
set (PACKAGE_NAME           ${CPACK_PACKAGE_NAME})
set (PACKAGE_VERSION        ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
if (CPACK_PACKAGE_VERSION_PATCH)
  set (PACKAGE_VERSION       ${PACKAGE_VERSION}.${CPACK_PACKAGE_VERSION_PATCH})
endif (CPACK_PACKAGE_VERSION_PATCH)
set (PACKAGE_BUGREPORT      bugs@openturns.org)
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION})

# The tests can't be run if this function is absent
enable_testing ()
add_custom_target (tests COMMENT "Build tests")
add_custom_target (check COMMENT "Run pre-installation tests")
add_custom_target (installcheck COMMENT "Run post-installation tests")

if (BUILD_PYTHON)
  find_package (SWIG 4)
  if (SWIG_FOUND)
    include (${SWIG_USE_FILE})

    # this name was chosen in common with pyagrum for otagrum
    set (OPENTURNS_SWIG_DEFINITIONS -DSWIG_TYPE_TABLE=pyproba)
  endif ()

  option (USE_PYTHON_SABI "Use Python stable ABI" OFF)
  if (USE_PYTHON_SABI AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.26)
    find_package (Python 3.6 COMPONENTS Interpreter Development.SABIModule)
    add_library (Python::Module ALIAS Python::SABIModule)
  else ()
    find_package (Python 3.6 COMPONENTS Interpreter Development.Module)
  endif ()

  if (Python_FOUND)
    include (FindPythonModule)
    find_python_module (numpy)
    find_python_module (scipy)
    find_python_module (pandas)
    find_python_module (matplotlib 3)
    find_python_module (chaospy)
    find_python_module (dill)

    if (USE_SPHINX)
      find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
      if (NOT SPHINX_EXECUTABLE)
        message (STATUS "Could NOT find sphinx-build executable")
      endif ()
      find_program (DVISVGM_EXECUTABLE NAMES dvisvgm DOC "dvi to svg converter")
      find_program (DVIPNG_EXECUTABLE NAMES dvipng DOC "dvi to png converter")
      if (NOT (DVISVGM_EXECUTABLE OR DVIPNG_EXECUTABLE))
        message (STATUS "Could NOT find dvisvgm|dvipng executable")
      endif ()
      if (SPHINX_EXECUTABLE AND (DVISVGM_EXECUTABLE OR DVIPNG_EXECUTABLE))
        find_python_module (sphinx 1.8)
        find_python_module (numpydoc 0.9)
        find_python_module (sphinx_gallery 0.7)
        find_python_module (sphinx_copybutton)
        find_python_module (sphinxcontrib.jquery)
      endif ()
      if (NOT NUMPYDOC_FOUND OR NOT SPHINX_GALLERY_FOUND OR NOT SPHINX_COPYBUTTON_FOUND OR NOT SPHINXCONTRIB.JQUERY_FOUND OR NOT MATPLOTLIB_FOUND OR NOT LIBXML2_FOUND)
        set (SPHINX_FOUND FALSE)
        message (STATUS "Disabling documentation (need dvisvgm/numpydoc/sphinx-gallery/sphinx-copybutton/sphinxcontrib-jquery/matplotlib/libxml2)")
      endif ()
    endif ()

    if (WIN32)
      set (OPENTURNS_PYTHON_MODULE_PATH Lib/site-packages CACHE STRING "site-packages dir")
    else ()
      set (OPENTURNS_PYTHON_MODULE_PATH ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages CACHE STRING "site-packages dir")
    endif ()
  endif ()
endif ()



# Define variables without OPENTURNS_ prefix with absolute paths
foreach (_var INSTALL_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH SYSCONFIG_PATH DATA_PATH DOC_PATH PYTHON_MODULE_PATH)
  if (IS_ABSOLUTE "${OPENTURNS_${_var}}")
    set (${_var} ${OPENTURNS_${_var}})
  else ()
    set (${_var} ${CMAKE_INSTALL_PREFIX}/${OPENTURNS_${_var}})
  endif ()
  get_filename_component (${_var} "${${_var}}" ABSOLUTE)
  file (TO_NATIVE_PATH "${${_var}}" NATIVE_${_var})
  string (REPLACE "\\" "\\\\" NATIVE_${_var} ${NATIVE_${_var}})
endforeach (_var)

# RPATH settings
set (CMAKE_INSTALL_RPATH ${LIBRARY_PATH})
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set (OPENTURNS_SWIG_INCLUDE_DIRS ${INCLUDE_PATH}/openturns/swig)

add_subdirectory (lib)

if (Python_FOUND AND SWIG_FOUND)
  add_subdirectory (python)
endif ()


include (CPack)

install (FILES COPYING
                COPYING.LESSER
                COPYING.cobyla
                COPYING.dsfmt
                COPYING.ev3
                COPYING.exprtk
                COPYING.faddeeva
                COPYING.fastgl
                COPYING.kendall
                COPYING.kissfft
                COPYING.cephes
                COPYING.tnc
          DESTINATION ${OPENTURNS_DOC_PATH}
       )

include(FeatureSummary)
feature_summary(WHAT ALL)
