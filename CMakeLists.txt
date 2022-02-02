#                                               -*- cmake -*-

cmake_minimum_required (VERSION 3.2)

# By default, build in Release mode. Must appear before project() command
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" Release Debug RelWithDebInfo MinSizeRel)
set (CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_BUILD_TYPE})

project (OpenTURNS)

option (USE_BISON                    "Looks for Bison if true and then build parser"                         ON)
option (USE_FLEX                     "Looks for Flex if true and then build lexer"                           ON)
option (USE_TBB                      "Use Intel Threading Building Blocks library for multithreading"        ON)
option (USE_HMAT                     "Use HMat library"                                                      ON)
option (USE_MUPARSER                 "Use muParser library"                                                  ON)
option (USE_EXPRTK                   "Use ExprTK library"                                                    ON)
option (USE_LIBXML2                  "Use LibXml2 for XML support"                                           ON)
option (USE_HDF5                     "Use HDF5 for high volume storage"                                      ON)
option (USE_R                        "Use R for graph output support"                                        ON)
option (USE_BOOST                    "Use Boost for distribution computation"                                ON)
option (USE_MPFR                     "Use MPFR for real valued special functions computation"                ON)
option (USE_MPC                      "Use MPC for complex valued special functions computation"              ON)
option (USE_SPHINX                   "Use sphinx for documentation"                                          ON)
option (USE_DOXYGEN                  "Use Doxygen for API documentation"                                     ON)
option (USE_NLOPT                    "Use NLopt for additional optimization algorithms"                      ON)
option (USE_CERES                    "Use Ceres Solver for additional optimization algorithms"               ON)
option (USE_CMINPACK                 "Use CMinpack for additional optimization algorithms"                   ON)
option (USE_DLIB                     "Use dlib for additional optimization algorithms"                       ON)
option (USE_COTIRE                   "Use cotire for unity builds"                                           OFF)
option (USE_IPOPT                    "Use Ipopt for MINLP problems"                                          ON)
option (USE_BONMIN                   "Use Bonmin for MINLP problems"                                         ON)
option (USE_SPECTRA                  "Use Spectra for eigenvalues computation"                               ON)
option (USE_PRIMESIEVE               "Use primesieve for prime numbers generation"                           ON)
option (USE_OPENMP                   "Use OpenMP to disable threading"                                       ON)
option (USE_OPENBLAS                 "Use OpenBLAS to disable threading"                                     ON)

option (BUILD_PYTHON                 "Build the python module for the library"                               ON)
option (BUILD_SHARED_LIBS            "Build shared libraries"                                                ON)

# Defines our own module path
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (POLICY CMP0022)
  # INTERFACE_LINK_LIBRARIES defines the link interface
  cmake_policy (SET CMP0022 NEW)
endif()

if (POLICY CMP0068)
  # RPATH settings on macOS do not affect install_name
  cmake_policy (SET CMP0068 NEW)
endif ()

if (POLICY CMP0093)
  # FindBoost reports Boost_VERSION in x.y.z format
  cmake_policy (SET CMP0093 NEW)
endif ()

if (USE_COTIRE)
  if (POLICY CMP0058)
    # Ninja requires custom command byproducts to be explicit
    cmake_policy (SET CMP0058 NEW)
  endif()
  include (cotire)
  # exclude dirty third-party sources (static vars, macros, ...)
  set (COTIRE_UNITY_SOURCE_EXCLUDE_EXTENSIONS "c;cc")
endif ()


# add library with unity build support, inspired by https://github.com/onqtam/ucm
macro (ot_add_library name)
  if (USE_COTIRE)
    set (orig_target ${name}_orig)
    # add regular target with another name
    add_library (${orig_target} ${ARGN})
    set_target_properties (${orig_target} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
    # set unity target name with the original name
    set_target_properties (${orig_target} PROPERTIES COTIRE_UNITY_TARGET_NAME ${name})
    # add unity build target
    cotire (${orig_target})
    set_target_properties (${name} PROPERTIES EXCLUDE_FROM_ALL 0 EXCLUDE_FROM_DEFAULT_BUILD 0)
    set_target_properties (${name} PROPERTIES OUTPUT_NAME ${name})
  else ()
    add_library (${name} ${ARGN})
  endif ()
endmacro ()

set (OPENTURNS_HOME_ENV_VAR OPENTURNS_HOME    )

if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set (CMAKE_INSTALL_LIBDIR lib${LIB_SUFFIX})
endif ()

set (OPENTURNS_LIBRARY_PATH
      ${CMAKE_INSTALL_LIBDIR}
      CACHE PATH "The directory where the library is installed"
   )
set (OPENTURNS_INCLUDE_PATH
      include
      CACHE PATH "The directory where the header files are installed"
   )
set (OPENTURNS_CONFIG_CMAKE_PATH
      ${CMAKE_INSTALL_LIBDIR}/cmake/openturns
      CACHE PATH "The directory where the CMake files are installed"
   )
set (OPENTURNS_SYSCONFIG_PATH
      etc
      CACHE PATH "The directory where the configuration file is installed"
   )
set (OPENTURNS_DATA_PATH
      share
      CACHE PATH "The directory where the common files are installed"
   )
set (OPENTURNS_DOC_PATH
      share/openturns/doc
      CACHE PATH "The directory where the license files are installed"
   )

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

set (CMAKE_CXX_STANDARD 11)

if (NOT DEFINED LAPACK_LIBRARIES)
  find_package (LAPACK REQUIRED)
  if (LAPACK_FOUND)
    set (OPENTURNS_HAVE_LAPACK TRUE)
  endif ()
endif ()
list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${LAPACK_LIBRARIES})

if (USE_SPECTRA)
  find_package (Spectra 1.0 CONFIG)
endif ()
if (Spectra_FOUND)
  message(STATUS "Found Spectra: ${Spectra_DIR} (found suitable version \"${Spectra_VERSION}\")")
  set (OPENTURNS_HAVE_SPECTRA TRUE)
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES Spectra::Spectra)
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
  else ()
    # fallback to our macro if primesieveConfig is not provided
    find_package (primesieve)
    if (primesieve_FOUND)
      set (OPENTURNS_HAVE_PRIMESIEVE TRUE)
      list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${PRIMESIEVE_INCLUDE_DIRS})
      list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${PRIMESIEVE_LIBRARIES})
    endif ()
  endif ()
endif ()

if (USE_BISON)
  find_package (BISON 2.4)
  if (BISON_FOUND)
    set (OPENTURNS_HAVE_BISON TRUE)
  endif ()
endif ()

if (USE_FLEX)
  find_package (FLEX)
  if (FLEX_FOUND)
    set (OPENTURNS_HAVE_FLEX TRUE)
  endif ()
endif ()

if (USE_TBB)
  find_package (TBB CONFIG)
  if (TBB_FOUND)
    message (STATUS "Found TBB: ${TBB_DIR} (found version ${TBB_VERSION})")
    set (TBB_LIBRARIES TBB::tbb)
  else ()
    # fallback to our macro
    find_package (TBB)
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
endif ()

if (USE_MUPARSER)
  find_package (muParser 2.2.3 QUIET)
endif ()
if (MUPARSER_FOUND)
  set (OPENTURNS_HAVE_MUPARSER TRUE)
  set (OPENTURNS_HAVE_ANALYTICAL_PARSER TRUE)
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${MUPARSER_LIBRARIES})
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${MUPARSER_INCLUDE_DIRS})
  set (SYMBOLICPARSER_DEFAULT_BACKEND MuParser)
endif ()
if (USE_EXPRTK)
  set (OPENTURNS_HAVE_EXPRTK TRUE)
  set (OPENTURNS_HAVE_ANALYTICAL_PARSER TRUE)
  set (SYMBOLICPARSER_DEFAULT_BACKEND ExprTk)
endif ()

if (USE_HMAT)
  find_package (HMAT 1.7 CONFIG)
endif ()
if (HMAT_FOUND)
  message(STATUS "Found HMAT: ${HMAT_DIR} (found suitable version \"${HMAT_VERSION}\")")
  set (OPENTURNS_HAVE_HMAT TRUE)
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${HMAT_INCLUDE_DIRS})
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${HMAT_LIBRARIES})
endif ()

if (USE_DOXYGEN)
  find_package (Doxygen QUIET)
endif ()

if (USE_LIBXML2)
  find_package (LibXml2)
endif ()
if (LIBXML2_FOUND)
  set (OPENTURNS_HAVE_LIBXML2 TRUE)
  list (APPEND OPENTURNS_DEFINITIONS ${LIBXML2_DEFINITIONS})
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR})
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${LIBXML2_LIBRARIES})
endif ()

if (USE_HDF5)
  find_package(HDF5 COMPONENTS CXX)
endif ()
if (HDF5_FOUND)
  set (OPENTURNS_HAVE_HDF5 TRUE)
  list (APPEND OPENTURNS_DEFINITIONS ${HDF5_DEFINITIONS})
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS})
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${HDF5_LIBRARIES})
endif ()

if (MSVC OR WINTEL)
  set (CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/pthread_win32 ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)
  include_directories (${PROJECT_SOURCE_DIR}/lib/include/pthread_win32 ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)
endif ()
find_package (Threads)
if (Threads_FOUND)
  list (APPEND OPENTURNS_INCLUDE_DIRS ${THREADS_INCLUDE_DIRS})
  list (APPEND OPENTURNS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (USE_BOOST)
  find_package (Boost 1.46)
  if (Boost_FOUND)
    if (CMAKE_VERSION VERSION_LESS 3.15)
      # Boost_VERSION reports the integer BOOST_VERSION from boost/version.hpp instead of x.y.z format
      set (Boost_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
    endif ()
    set (OPENTURNS_HAVE_BOOST TRUE)
    if (USE_MPFR)
      find_package (MPFR)
      if (MPFR_FOUND)
        set (OPENTURNS_HAVE_MPFR TRUE)
        list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${MPFR_INCLUDE_DIRS})
        list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${MPFR_LIBRARIES})
      endif ()
    endif ()
    if (USE_MPC AND NOT Boost_VERSION VERSION_LESS 1.68)
      find_package (MPC)
      if (MPC_FOUND)
        set (OPENTURNS_HAVE_MPC TRUE)
        list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${MPC_INCLUDE_DIRS})
        list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${MPC_LIBRARIES})
      endif ()
    endif ()
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
  endif ()
endif ()

if (USE_R)
  find_package (R COMPONENTS base QUIET)
endif ()
if (NOT R_EXECUTABLE)
  set (R_EXECUTABLE "R${CMAKE_EXECUTABLE_SUFFIX}")
endif ()

if (USE_NLOPT)
  find_package (NLopt CONFIG QUIET)
  if (NLopt_FOUND)
    message(STATUS "Found NLopt: ${NLopt_DIR}  (found version \"${NLopt_VERSION}\")")
  else ()
    # fallback to our macro if NLoptConfig is not provided
    find_package (NLopt)
    set (NLopt_FOUND ${NLOPT_FOUND})
  endif ()
  if (NLopt_FOUND)
    set (OPENTURNS_HAVE_NLOPT TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${NLOPT_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${NLOPT_LIBRARIES})

    try_run (_NLOPT_STOGO_RUNS _NLOPT_STOGO_BUILDS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/nlopt_have_stogo.cxx LINK_LIBRARIES ${NLOPT_LIBRARIES})
    if (_NLOPT_STOGO_BUILDS)
      if (_NLOPT_STOGO_RUNS EQUAL 0)
        set (OPENTURNS_NLOPT_HAVE_STOGO TRUE)
      else ()
        message (STATUS "nlopt::GD_STOGO: fails to run")
      endif ()
    else ()
      message (STATUS "nlopt::GD_STOGO: fails to build")
    endif ()

    try_run (_NLOPT_AGS_RUNS _NLOPT_AGS_BUILDS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/nlopt_have_ags.cxx LINK_LIBRARIES ${NLOPT_LIBRARIES})
    if (_NLOPT_AGS_BUILDS)
      if (_NLOPT_AGS_RUNS EQUAL 0)
        set (OPENTURNS_NLOPT_HAVE_AGS TRUE)
      else ()
        message (STATUS "nlopt::GN_AGS: fails to run")
      endif ()
    else ()
      message (STATUS "nlopt::GN_AGS: fails to build")
    endif ()

  endif ()
endif ()

if (USE_DLIB)
  find_package (dlib CONFIG)
  if (dlib_FOUND)
    set (OPENTURNS_HAVE_DLIB TRUE)
    message (STATUS "Found dlib: ${dlib_DIR} (found version ${dlib_VERSION})")
    if (dlib_VERSION VERSION_GREATER 19.7)
      set (OPENTURNS_HAVE_DLIB_GLOBAL_OPTIMIZATION TRUE)
    endif ()
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES dlib::dlib)
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
  endif ()
endif ()

if (USE_BONMIN)
  find_package (Bonmin)
  if (Bonmin_FOUND)
    set (OPENTURNS_HAVE_BONMIN TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${COIN_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${COIN_BONMIN_LIBRARIES})
  endif ()
endif ()

if (USE_IPOPT)
  find_package (Ipopt)
  if (Ipopt_FOUND)
    set (OPENTURNS_HAVE_IPOPT TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${IPOPT_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${IPOPT_LIBRARIES})
  endif ()
endif ()

if (USE_OPENMP)
  find_package (OpenMP)
  if (OPENMP_FOUND)
    set (OPENTURNS_HAVE_OPENMP TRUE)
    if (TARGET OpenMP::OpenMP_CXX)
      list (APPEND OPENTURNS_PRIVATE_LIBRARIES OpenMP::OpenMP_CXX)
    else ()
      set (CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
      set (CMAKE_SHARED_LINKER_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
    endif ()
  endif ()
endif ()

if (MSVC)
  # Disable some warnings
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4197 /wd4244 /wd4251 /wd4267 /wd4275 /wd4996")
endif ()

if (CMAKE_VERSION VERSION_LESS 3.20)
  include (TestBigEndian)
  test_big_endian (__BIG_ENDIAN__)
else ()
  if ("${CMAKE_CXX_BYTE_ORDER}" STREQUAL "BIG_ENDIAN")
    set (__BIG_ENDIAN__ 1)
  endif ()
endif ()

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
if (POLICY CMP0078)
  # swig standard target names
  cmake_policy(SET CMP0078 NEW)
endif ()
if (POLICY CMP0086)
  # UseSWIG honors SWIG_MODULE_NAME via -module flag
  cmake_policy(SET CMP0086 NEW)
endif ()

try_compile(OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/unsignedlong_same_as_uint64_t.cxx)

try_run (_HAVE_FR_LOC_RUNS _HAVE_FR_LOC_BUILDS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/have_fr_loc.cxx)
if (_HAVE_FR_LOC_BUILDS)
  if (NOT _HAVE_FR_LOC_RUNS EQUAL 0)
    set (OPENTURNS_HAVE_FR_LOC TRUE)
    message (STATUS "French locale found")
  else()
    set (OPENTURNS_HAVE_FR_LOC FALSE)
    message (STATUS "French locale not found")
  endif ()
endif ()

try_run (_HAVE_STD_REGEX_RUNS _HAVE_STD_REGEX_BUILDS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/have_std_regex.cxx)
if (_HAVE_STD_REGEX_BUILDS)
  if (NOT _HAVE_STD_REGEX_RUNS EQUAL 0)
    set (OPENTURNS_HAVE_STD_REGEX TRUE)
    message (STATUS "std::regex found")
  else()
    set (OPENTURNS_HAVE_STD_REGEX FALSE)
    message (STATUS "std::regex not found")
  endif ()
endif ()

# Find system prerequisites
include (CheckIncludeFile)
check_include_file (dirent.h      OPENTURNS_HAVE_DIRENT_H     )
check_include_file (dlfcn.h       OPENTURNS_HAVE_DLFCN_H      )
check_include_file (libgen.h      OPENTURNS_HAVE_LIBGEN_H     )
check_include_file (pthread.h     OPENTURNS_HAVE_PTHREAD_H    )
check_include_file (stdlib.h      OPENTURNS_HAVE_STDLIB_H     )
check_include_file (sys/stat.h    OPENTURNS_HAVE_SYS_STAT_H   )
check_include_file (sys/types.h   OPENTURNS_HAVE_SYS_TYPES_H  )
check_include_file (unistd.h      OPENTURNS_HAVE_UNISTD_H     )
check_include_file (xlocale.h     OPENTURNS_HAVE_XLOCALE_H    )

include (CheckSymbolExists)
if(OPENTURNS_HAVE_XLOCALE_H)
check_symbol_exists(uselocale "locale.h;xlocale.h"  OPENTURNS_HAVE_USELOCALE )
else()
check_symbol_exists(uselocale "locale.h"  OPENTURNS_HAVE_USELOCALE )
endif()

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

set (BUILT_SOURCEFILES "" CACHE INTERNAL "List of source files to compile")
macro (ot_add_built_source_file FILENAME)
  set (sf ${BUILT_SOURCEFILES} ${FILENAME})
  set (BUILT_SOURCEFILES ${sf} CACHE INTERNAL "List of built source files to compile")
endmacro (ot_add_built_source_file)

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

# Add definitions according to the prerequisites found
if (NOT BUILD_SHARED_LIBS)
  list (APPEND OPENTURNS_DEFINITIONS -DOT_STATIC)
endif ()

if (MSVC OR WINTEL)
  list (APPEND OPENTURNS_DEFINITIONS -D_USE_MATH_DEFINES)
endif ()

add_definitions (${OPENTURNS_DEFINITIONS})
if (OPENTURNS_INCLUDE_DIRS)
  list (REMOVE_DUPLICATES OPENTURNS_INCLUDE_DIRS)
endif (OPENTURNS_INCLUDE_DIRS)
include_directories (${OPENTURNS_INCLUDE_DIRS})
include_directories (${OPENTURNS_PRIVATE_INCLUDE_DIRS})

set (CPACK_PACKAGE_NAME          openturns     )
set (CPACK_PACKAGE_VERSION_MAJOR 1             )
set (CPACK_PACKAGE_VERSION_MINOR 19dev)
set (CPACK_PACKAGE_VERSION_PATCH               )
set (CPACK_SOURCE_GENERATOR      "TGZ;TBZ2"    )
set (CPACK_BINARY_STGZ    OFF CACHE BOOL "STGZ")
set (CPACK_BINARY_TBZ2    ON  CACHE BOOL "TBZ2")
set (CPACK_BINARY_TGZ     ON  CACHE BOOL "TGZ" )
set (CPACK_BINARY_TZ      OFF CACHE BOOL "TZ"  )
set (CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;${CPACK_SOURCE_IGNORE_FILES}")


# Set global definitions
# TODO: remove PACKAGE_<XX> from source code. Needed for compatibility with autotools
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
  find_package (SWIG 3)
  if (SWIG_FOUND)
    include (${SWIG_USE_FILE})
    include (TargetLinkLibrariesWithDynamicLookup)

    # this name was chosen in common with pyagrum for otagrum
    set (OPENTURNS_SWIG_DEFINITIONS -DSWIG_TYPE_TABLE=pyproba)

    # ptr conversion argument to handle None correctly in swig 4.x, this just allows to pass compilation with swig 3.x
    if (SWIG_VERSION VERSION_LESS 4)
      list (APPEND OPENTURNS_SWIG_DEFINITIONS -DSWIG_POINTER_NO_NULL=0x0)
    endif ()
  endif ()

  find_package (PythonInterp)
  if (PYTHONINTERP_FOUND)
    find_package (PythonLibs ${PYTHON_VERSION_STRING} EXACT)
    include (FindPythonModule)
    find_python_module (numpy)
    find_python_module (scipy)
    find_python_module (pandas)
    find_python_module (matplotlib 1.3.1)
    find_python_module (chaospy)
    find_python_module (dill)

    if (USE_SPHINX)
      find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
      if (SPHINX_EXECUTABLE)
        find_python_module (sphinx 1.1)
        find_python_module (numpydoc 0.9) # v0.7 & v0.8 have bugs for citations
        find_python_module (sphinx_gallery 0.7)
        if (NOT Bonmin_FOUND)
          set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_bonmin)")
        endif ()
        if (NOT Ceres_FOUND AND NOT CMinpack_FOUND)
          set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(calibration_flooding)|(calibration_chaboche)|(calibration_deflection_tube)")
        endif ()
        if (NOT dlib_FOUND)
          set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_dlib)")
        endif ()
        if (NOT PANDAS_FOUND)
          set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(sample_pandas)")
        endif ()
        if (NOT HMAT_FOUND)
          set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(kriging_cantilever_beam_hmat)")
          set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(gaussian_process_covariance_hmat)")
        endif ()
        if (NOT HDF5_FOUND)
          set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(study_save_load)")
        endif ()
      else ()
        message (STATUS "Could NOT find Sphinx executable")
      endif ()
      if (NOT NUMPYDOC_FOUND OR NOT SPHINX_GALLERY_FOUND OR NOT MATPLOTLIB_FOUND OR NOT LIBXML2_FOUND OR NOT NLopt_FOUND)
        set (SPHINX_FOUND FALSE)
        message (STATUS "Disabling documentation (need numpydoc/sphinx-gallery/matplotlib/libxml2/nlopt)")
      endif ()
    endif ()

    if (DEFINED PYTHON_SITE_PACKAGES)
      set (OPENTURNS_PYTHON_MODULE_PATH "${PYTHON_SITE_PACKAGES}")
    else ()
     if (WIN32)
       set (OPENTURNS_PYTHON_MODULE_PATH Lib/site-packages)
     else ()
       set (OPENTURNS_PYTHON_MODULE_PATH ${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
      endif ()
    endif ()

    set (OPENTURNS_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH "${OPENTURNS_PYTHON_MODULE_PATH}")
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

if (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
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
                COPYING.hmat
                COPYING.kendall
                COPYING.kissfft
                COPYING.KolmogorovSmirnovDist
                COPYING.poissinv
                COPYING.tnc
          DESTINATION ${OPENTURNS_DOC_PATH}
       )

include(FeatureSummary)
feature_summary(WHAT ALL)
