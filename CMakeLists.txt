#                                               -*- cmake -*-

cmake_minimum_required (VERSION 2.8.8)

# By default, build in Release mode. Must appear before project() command
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" Release Debug RelWithDebInfo MinSizeRel)

project (OpenTURNS)

option (USE_BISON                    "Looks for Bison if true and then build parser"                         ON)
option (USE_FLEX                     "Looks for Flex if true and then build lexer"                           ON)
option (USE_TBB                      "Use Intel Threading Building Blocks library for multithreading"        ON)
option (USE_HMAT                     "Use HMat library"                                                      ON)
option (USE_MUPARSER                 "Use muParser library"                                                  ON)
option (USE_EXPRTK                   "Use ExprTK library"                                                    ON)
option (USE_LIBXML2                  "Use LibXml2 for XML support"                                           ON)
option (USE_R                        "Use R for graph output support"                                        ON)
option (USE_BOOST                    "Use Boost for distribution computation"                                ON)
option (USE_SPHINX                   "Use sphinx for documentation"                                          ON)
option (USE_NBSPHINX                 "Use nbsphinx for notebook examples documentation"                      ON)
option (USE_DOXYGEN                  "Use Doxygen for API documentation"                                     ON)
option (USE_NLOPT                    "Use NLopt for additional optimization algorithms"                      ON)
option (USE_OPTPP                    "Use OPT++ for additional optimization algorithms"                      ON)
option (USE_COTIRE                   "Use cotire for unity builds"                                           OFF)

option (BUILD_PYTHON                 "Build the python module for the library"                               ON)
option (BUILD_VALIDATION             "Build the validation files of the library"                             ON)
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
set (OPENTURNS_EXAMPLE_PATH
      share/openturns/examples
      CACHE PATH "The directory where the example files are installed"
   )
set (OPENTURNS_VALIDATION_PATH
      share/openturns/validation
      CACHE PATH "The directory where validation files are installed"
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


# CMake 2.x does not define MSVC when using Intel compiler on Windows, see
# http://public.kitware.com/Bug/view.php?id=14476
if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(WINTEL TRUE)
endif()


if (NOT DEFINED LAPACK_LIBRARIES)
  find_package (LAPACK REQUIRED)
  if (LAPACK_FOUND)
    set (OPENTURNS_HAVE_LAPACK TRUE)
  endif ()
endif ()
list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${LAPACK_LIBRARIES})


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
  find_package (TBB)
endif ()
if (TBB_FOUND)
  set (OPENTURNS_HAVE_TBB_TBB_H TRUE)
  set (OPENTURNS_HAVE_TBB TRUE)
  list (APPEND OPENTURNS_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
  list (APPEND OPENTURNS_LIBRARIES ${TBB_LIBRARIES})
  if (MSVC OR WINTEL)
    set (OPENTURNS_TBB_NO_IMPLICIT_LINKAGE ON CACHE BOOL "Prevent implicit linkage against tbb.lib ")
  endif ()
endif ()

if (USE_MUPARSER)
  find_package (muParser 2.2.3)
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
  find_package (HMAT 1.2 NO_MODULE)
endif ()
if (HMAT_FOUND)
  set (OPENTURNS_HAVE_HMAT TRUE)
  list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${HMAT_LIBRARIES})
  list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${HMAT_INCLUDE_DIRS})
endif ()

if (USE_DOXYGEN)
  find_package (Doxygen)
endif ()

if (USE_LIBXML2)
  find_package (LibXml2)
endif ()
if (LIBXML2_FOUND)
  set (OPENTURNS_HAVE_LIBXML2 TRUE)
  list (APPEND OPENTURNS_DEFINITIONS ${LIBXML2_DEFINITIONS})
  list (APPEND OPENTURNS_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR})
  list (APPEND OPENTURNS_LIBRARIES ${LIBXML2_LIBRARIES})
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

if (MSVC OR WINTEL)
  set (BOOST_REQUIRED_ARG REQUIRED)
  set (USE_BOOST ON CACHE INTERNAL "Force use of Boost on Windows")
endif ()
if (USE_BOOST)
  # doesn't pass all tests with boost <=1.42 (debian6)
  # but at least pass with 1.46 (ubuntu precise)
  find_package (Boost 1.46 ${BOOST_REQUIRED_ARG})
  if (Boost_FOUND)
    set (OPENTURNS_HAVE_BOOST TRUE)

    # see SpecFunc.hxx
    if (MSVC)
      list (APPEND OPENTURNS_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    else ()
      list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    endif ()
  endif ()
endif ()

if (USE_R)
  find_package (R COMPONENTS base)
endif ()
if (NOT R_EXECUTABLE)
  set (R_EXECUTABLE "R${CMAKE_EXECUTABLE_SUFFIX}")
endif ()

if (USE_NLOPT)
  find_package (NLopt)
  if (NLOPT_FOUND)
    set (OPENTURNS_HAVE_NLOPT TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${NLOPT_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${NLOPT_LIBRARIES})
  endif ()
endif ()

if (USE_OPTPP)
  find_package (OPTpp)
  if (OPTPP_FOUND)
    set (OPENTURNS_HAVE_OPTPP TRUE)
    list (APPEND OPENTURNS_PRIVATE_INCLUDE_DIRS ${OPTPP_INCLUDE_DIRS})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${OPTPP_LIBRARIES})
  endif ()
endif ()

if (MSVC)
  # Disable some warnings
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4197 /wd4244 /wd4251 /wd4267 /wd4996")
endif ()

include (TestBigEndian)
test_big_endian (__BIG_ENDIAN__) 

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

try_compile (OPENTURNS_HAVE_SYNC_BUILTINS
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc_sync_builtins.cxx)

# Find system prerequisites
include (CheckIncludeFile)
check_include_file (assert.h      OPENTURNS_HAVE_ASSERT_H     )
check_include_file (dirent.h      OPENTURNS_HAVE_DIRENT_H     )
check_include_file (dlfcn.h       OPENTURNS_HAVE_DLFCN_H      )
check_include_file (errno.h       OPENTURNS_HAVE_ERRNO_H      )
check_include_file (inttypes.h    OPENTURNS_HAVE_INTTYPES_H   )
check_include_file (libgen.h      OPENTURNS_HAVE_LIBGEN_H     )
check_include_file (math.h        OPENTURNS_HAVE_MATH_H       )
check_include_file (pthread.h     OPENTURNS_HAVE_PTHREAD_H    )
check_include_file (signal.h      OPENTURNS_HAVE_SIGNAL_H     )
check_include_file (stdbool.h     OPENTURNS_HAVE_STDBOOL_H    )
check_include_file (stdint.h      OPENTURNS_HAVE_STDINT_H     )
check_include_file (stdlib.h      OPENTURNS_HAVE_STDLIB_H     )
check_include_file (string.h      OPENTURNS_HAVE_STRING_H     )
check_include_file (strings.h     OPENTURNS_HAVE_STRINGS_H    )
check_include_file (sys/stat.h    OPENTURNS_HAVE_SYS_STAT_H   )
check_include_file (sys/time.h    OPENTURNS_HAVE_SYS_TIME_H   )
check_include_file (sys/types.h   OPENTURNS_HAVE_SYS_TYPES_H  )
check_include_file (sys/un.h      OPENTURNS_HAVE_SYS_UN_H     )
check_include_file (unistd.h      OPENTURNS_HAVE_UNISTD_H     )
check_include_file (xlocale.h     OPENTURNS_HAVE_XLOCALE_H    )

include (CheckSymbolExists)
if(OPENTURNS_HAVE_XLOCALE_H)
check_symbol_exists(uselocale "locale.h;xlocale.h"  OPENTURNS_HAVE_USELOCALE )
else()
check_symbol_exists(uselocale "locale.h"  OPENTURNS_HAVE_USELOCALE )
endif()

include (CheckFunctionExists)
if (OPENTURNS_HAVE_DLFCN_H)
  find_library (LIBDL_LIBRARIES NAMES dl)
  if (LIBDL_LIBRARIES)
    set (CMAKE_REQUIRED_LIBRARIES ${LIBDL_LIBRARIES})
    list (APPEND OPENTURNS_PRIVATE_LIBRARIES ${LIBDL_LIBRARIES})
  endif ()
  check_function_exists (dladdr OPENTURNS_HAVE_DLADDR)
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
set (CPACK_PACKAGE_VERSION_MINOR 13dev)
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

# uninstall target
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY
)

add_custom_target (uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# The tests can't be run if this function is absent
enable_testing ()
add_custom_target (tests COMMENT "Build tests")
add_custom_target (check COMMENT "Run pre-installation tests")
add_custom_target (installcheck COMMENT "Run post-installation tests")

if (BUILD_PYTHON)
  # workaround for cmake bug #0013449
  if (CMAKE_VERSION VERSION_LESS 3 AND DEFINED CMAKE_FIND_ROOT_PATH)
    find_program (SWIG_EXECUTABLE NAMES swig2.0 swig)
    if (SWIG_EXECUTABLE)
      set (SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
      set (SWIG_FOUND TRUE)
      set (SWIG_VERSION 2)
    endif ()
  else ()
    # we need absolute module imports
    find_package (SWIG 2.0.9)
  endif ()
  if (SWIG_FOUND)
    include (${SWIG_USE_FILE})
    include (TargetLinkLibrariesWithDynamicLookup)
  endif ()

  find_package (PythonInterp)
  find_package (PythonLibs)
  include (FindPythonModule)
  find_python_module (numpy)
  find_python_module (scipy)
  find_python_module (chaospy)
  find_python_module (matplotlib 1.1)
  find_python_module (nbconvert)

  if (USE_SPHINX)
    find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
    if (SPHINX_EXECUTABLE)
      find_python_module (sphinx 1.1)
      find_python_module (numpydoc)
      if (USE_NBSPHINX)
        find_program (PANDOC_EXECUTABLE NAMES pandoc DOC "a universal document converter")
        if (PANDOC_EXECUTABLE)
          find_python_module (nbsphinx)
          find_python_module (IPython)
        else ()
          message (STATUS "Could NOT find Pandoc executable")
        endif ()
        if (NOT IPYTHON_FOUND OR SPHINX_VERSION_STRING VERSION_LESS 1.5.1)
          set (NBSPHINX_FOUND FALSE CACHE BOOL "")
          message (STATUS "Disabling notebook documentation examples (need pandoc/nbsphinx/ipython)")
        endif ()
      endif ()
    else ()
      message (STATUS "Could NOT find Sphinx executable")
    endif ()
    if (NOT NUMPYDOC_FOUND OR NOT OPENTURNS_HAVE_ANALYTICAL_PARSER OR NOT MATPLOTLIB_FOUND OR NOT LIBXML2_FOUND OR NOT NLOPT_FOUND OR NOT OPTPP_FOUND)
      set (SPHINX_FOUND FALSE)
      message (STATUS "Disabling documentation (need numpydoc/matplotlib/libxml2/nlopt/opt++)")
    endif ()
  endif ()

  if (PYTHONINTERP_FOUND)
    if (DEFINED PYTHON_SITE_PACKAGES)
      set (OPENTURNS_PYTHON_MODULE_PATH ${PYTHON_SITE_PACKAGES})

    else ()
      execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}'))"
                        OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
                        RESULT_VARIABLE _PYTHON_pythonlib_result
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

      if (_PYTHON_pythonlib_result)
        message (SEND_ERROR "Could not run ${PYTHON_EXECUTABLE}")
      endif ()

      get_filename_component (_ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE)
      file (RELATIVE_PATH OPENTURNS_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH})
    endif ()

    set (OPENTURNS_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH ${OPENTURNS_PYTHON_MODULE_PATH})
  endif ()
endif ()



# Define variables without OPENTURNS_ prefix with absolute paths
foreach (_var INSTALL_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH SYSCONFIG_PATH DATA_PATH EXAMPLE_PATH VALIDATION_PATH DOC_PATH PYTHON_MODULE_PATH)
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
set (OPENTURNS_SWIG_DEFINITIONS -DSWIG_TYPE_TABLE=openturns)

# necessary to link against Py_InitModule64
if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 8)
  set (OPENTURNS_SWIG_DEFINITIONS "-DMS_WIN64 ${OPENTURNS_SWIG_DEFINITIONS}")
endif ()


add_subdirectory (lib)

if (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
  add_subdirectory (python)
endif ()


if (BUILD_VALIDATION)
  add_subdirectory (validation)
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
