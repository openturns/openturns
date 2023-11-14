# - Find TBB
# Intel Threading Building Blocks offers a rich and complete approach to expressing parallelism in a C++ program
# www.threadingbuildingblocks.org
#
# The module defines the following variables:
#  TBB_FOUND - the system has TBB
#  TBB_INCLUDE_DIR - where to find tbb/tbb.h
#  TBB_INCLUDE_DIRS - tbb includes
#  TBB_LIBRARY - where to find the TBB library
#  TBB_LIBRARIES - aditional libraries
#  TBB_VERSION_STRING - version (ex. 2.2)
#  TBB_MAJOR_VERSION - major version
#  TBB_MINOR_VERSION - minor version
#  TBB_INTERFACE_VERSION - interface version
#  TBB_ROOT_DIR - root dir (ex. /usr/local)

#=============================================================================
# Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path (TBB_INCLUDE_DIR NAMES tbb/tbb.h DOC "TBB include directory")

set (TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})

# version
if (EXISTS ${TBB_INCLUDE_DIR}/oneapi/tbb/version.h)
  set (_VERSION_FILE ${TBB_INCLUDE_DIR}/oneapi/tbb/version.h)
elseif (EXISTS ${TBB_INCLUDE_DIR}/tbb/tbb_stddef.h)
  set (_VERSION_FILE ${TBB_INCLUDE_DIR}/tbb/tbb_stddef.h)
endif ()
if (DEFINED _VERSION_FILE)
  file (STRINGS ${_VERSION_FILE} _VERSION_MAJOR_STRING REGEX ".*define[ ]+TBB_VERSION_MAJOR[ ]+[0-9]+.*")
  file (STRINGS ${_VERSION_FILE} _VERSION_MINOR_STRING REGEX ".*define[ ]+TBB_VERSION_MINOR[ ]+[0-9]+.*")
  if (_VERSION_MAJOR_STRING AND _VERSION_MINOR_STRING)
    string (REGEX REPLACE ".*TBB_VERSION_MAJOR[ ]+([0-9]+)" "\\1" TBB_MAJOR_VERSION ${_VERSION_MAJOR_STRING})
    string (REGEX REPLACE ".*TBB_VERSION_MINOR[ ]+([0-9]+)" "\\1" TBB_MINOR_VERSION ${_VERSION_MINOR_STRING})
    set (TBB_VERSION_STRING "${TBB_MAJOR_VERSION}.${TBB_MINOR_VERSION}")
  endif ()
  file (STRINGS ${_VERSION_FILE} _VERSION_INTERFACE_STRING REGEX ".*define[ ]+TBB_INTERFACE_VERSION[ ]+[0-9]+.*")
  if (_VERSION_INTERFACE_STRING)
    string (REGEX REPLACE ".*TBB_INTERFACE_VERSION[ ]+([0-9]+)" "\\1" TBB_INTERFACE_VERSION ${_VERSION_INTERFACE_STRING})
  endif ()
endif ()

# check version
set (_TBB_VERSION_MATCH TRUE)
if (TBB_FIND_VERSION AND TBB_VERSION_STRING)
  if (TBB_FIND_VERSION_EXACT)
    if (${TBB_FIND_VERSION} VERSION_EQUAL ${TBB_VERSION_STRING})
    else()
      set (_TBB_VERSION_MATCH FALSE)
    endif ()
  else ()
    if (${TBB_FIND_VERSION} VERSION_GREATER ${TBB_VERSION_STRING})
      set (_TBB_VERSION_MATCH FALSE)
    endif ()
  endif ()
endif ()


find_library (TBB_LIBRARY NAMES tbb DOC "TBB library location")

set (TBB_LIBRARIES ${TBB_LIBRARY})

# root dir
# try to guess root dir from include dir
if (TBB_INCLUDE_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" TBB_ROOT_DIR ${TBB_INCLUDE_DIR})

# try to guess root dir from library dir
elseif (TBB_LIBRARY)
  string (REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" TBB_ROOT_DIR ${TBB_LIBRARY})
endif ()

# handle REQUIRED and QUIET options
include (FindPackageHandleStandardArgs)


find_package_handle_standard_args (TBB
  REQUIRED_VARS TBB_LIBRARY _TBB_VERSION_MATCH TBB_INCLUDE_DIR TBB_INCLUDE_DIRS TBB_LIBRARIES
  VERSION_VAR TBB_VERSION_STRING
)


mark_as_advanced (
  TBB_LIBRARY
  TBB_LIBRARIES
  TBB_INCLUDE_DIR
  TBB_INCLUDE_DIRS
  TBB_ROOT_DIR
  TBB_VERSION_STRING
  TBB_MAJOR_VERSION
  TBB_MINOR_VERSION
  TBB_INTERFACE_VERSION
)
