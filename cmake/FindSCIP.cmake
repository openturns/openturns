# - Find SCIP
# https://www.scipopt.org/
#
# The module defines the following variables:
#  SCIP_FOUND        - True if SCIP was found.
#  SCIP_INCLUDE_DIRS - where to find scip/scip.h.
#  SCIP_LIBRARIES    - Libraries to link against.
#  SCIP_VERSION      - Version of SCIP, when it can be determined.
#
# This module is only used as a fallback: the CMake configuration files
# installed by SCIP (scip-config.cmake) are preferred when available.
#
#=============================================================================
# Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

# Find the path based on a required header file
find_path (SCIP_INCLUDE_DIR NAMES scip/scip.h
  HINTS "${SCIP_ROOT_DIR}/include"
  PATH_SUFFIXES scip
)

find_library (SCIP_LIBRARY NAMES scip libscip
  HINTS "${SCIP_ROOT_DIR}/lib"
)

# try to guess root dir from include dir
if (SCIP_INCLUDE_DIR AND NOT DEFINED SCIP_ROOT_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" SCIP_ROOT_DIR ${SCIP_INCLUDE_DIR})
endif ()

# Version macros live either in a generated version.h, in the generated
# config.h, or directly in def.h
set (_SCIP_VERSION_HEADER)
foreach (_candidate IN ITEMS "${SCIP_INCLUDE_DIR}/scip/version.h" "${SCIP_INCLUDE_DIR}/scip/config.h" "${SCIP_INCLUDE_DIR}/scip/def.h")
  if (EXISTS "${_candidate}")
    set (_SCIP_VERSION_HEADER "${_candidate}")
    break ()
  endif ()
endforeach ()
if (_SCIP_VERSION_HEADER)
  file (STRINGS "${_SCIP_VERSION_HEADER}" _SCIP_VERSION_MAJOR REGEX "#define[ ]+SCIP_VERSION_MAJOR[ ]+[0-9]+")
  file (STRINGS "${_SCIP_VERSION_HEADER}" _SCIP_VERSION_MINOR REGEX "#define[ ]+SCIP_VERSION_MINOR[ ]+[0-9]+")
  file (STRINGS "${_SCIP_VERSION_HEADER}" _SCIP_VERSION_PATCH REGEX "#define[ ]+SCIP_VERSION_PATCH[ ]+[0-9]+")
  if (_SCIP_VERSION_MAJOR AND _SCIP_VERSION_MINOR AND _SCIP_VERSION_PATCH)
    string (REGEX REPLACE ".*SCIP_VERSION_MAJOR[ ]+([0-9]+).*" "\\1" _SCIP_VERSION_MAJOR "${_SCIP_VERSION_MAJOR}")
    string (REGEX REPLACE ".*SCIP_VERSION_MINOR[ ]+([0-9]+).*" "\\1" _SCIP_VERSION_MINOR "${_SCIP_VERSION_MINOR}")
    string (REGEX REPLACE ".*SCIP_VERSION_PATCH[ ]+([0-9]+).*" "\\1" _SCIP_VERSION_PATCH "${_SCIP_VERSION_PATCH}")
    set (SCIP_VERSION "${_SCIP_VERSION_MAJOR}.${_SCIP_VERSION_MINOR}.${_SCIP_VERSION_PATCH}")
  endif ()
endif ()

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (SCIP
  REQUIRED_VARS SCIP_LIBRARY SCIP_INCLUDE_DIR
  VERSION_VAR SCIP_VERSION
)

if (SCIP_FOUND)
  set (SCIP_INCLUDE_DIRS ${SCIP_INCLUDE_DIR})
  set (SCIP_LIBRARIES ${SCIP_LIBRARY})
endif ()

mark_as_advanced (SCIP_INCLUDE_DIR
  SCIP_LIBRARY
)
