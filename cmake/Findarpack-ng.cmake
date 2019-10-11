# - Find arpack-ng
# subroutines designed to solve large scale eigenvalue problems
# https://github.com/opencollab/arpack-ng
#
# The module defines the following variables:
#  arpack_ng_FOUND - the system has arpack-ng
#  arpack_ng_INCLUDE_DIR - where to find arpack.h
#  arpack_ng_INCLUDE_DIRS - arpack-ng includes
#  arpack_ng_LIBRARY - where to find the arpack-ng library
#  arpack_ng_LIBRARIES - aditional libraries
#  arpack_ng_ROOT_DIR - root dir (ex. /usr/local)
#
#=============================================================================
# Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

# set arpack_ng_INCLUDE_DIR
find_path (arpack_ng_INCLUDE_DIR
  NAMES arpack.hpp
  PATH_SUFFIXES arpack
  HINTS ${arpack_ng_ROOT_DIR}/include/arpack
  DOC
    "arpack-ng include directory"
)

# set arpack_ng_INCLUDE_DIRS
set (arpack_ng_INCLUDE_DIRS ${arpack_ng_INCLUDE_DIR})

# set arpack_ng_LIBRARY
find_library (arpack_ng_LIBRARY
  NAMES arpack
  HINTS ${arpack_ng_ROOT_DIR}/lib
  DOC
    "arpack-ng library location"
)

# set arpack_ng_LIBRARIES
set (arpack_ng_LIBRARIES ${arpack_ng_LIBRARY})

# root dir
# try to guess root dir from include dir
if (arpack_ng_INCLUDE_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" arpack_ng_ROOT_DIR ${arpack_ng_INCLUDE_DIR})

# try to guess root dir from library dir
elseif (arpack_ng_LIBRARY)
  string (REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" arpack_ng_ROOT_DIR ${arpack_ng_LIBRARY})
endif ()

# handle REQUIRED and QUIET options
include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (arpack-ng DEFAULT_MSG arpack_ng_LIBRARY
  arpack_ng_INCLUDE_DIR
  arpack_ng_INCLUDE_DIRS
  arpack_ng_LIBRARIES
  arpack_ng_ROOT_DIR
)

mark_as_advanced (
  arpack_ng_LIBRARY
  arpack_ng_LIBRARIES
  arpack_ng_INCLUDE_DIR
  arpack_ng_INCLUDE_DIRS
  arpack_ng_ROOT_DIR
)
