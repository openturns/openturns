# - Find muParser
# muParser is an extensible high performance math expression parser library written in C++
# http://muparser.sourceforge.net
#
# The module defines the following variables:
#  MUPARSER_FOUND        - True if muParser found.
#  MUPARSER_INCLUDE_DIRS - where to find muParser.h, etc.
#  MUPARSER_LIBRARIES    - List of libraries when using muParser.
#
#=============================================================================
# Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

if (MUPARSER_INCLUDE_DIR AND MUPARSER_LIBRARIES)
  # Already in cache, be silent
  set (muParser_FIND_QUIETLY TRUE)
endif ()

find_path (MUPARSER_INCLUDE_DIR muParser.h
            PATHS
            C:/muparser/include
            PATH_SUFFIXES muParser)

set (MUPARSER_INCLUDE_DIRS ${MUPARSER_INCLUDE_DIR})

# version
set (_VERSION_FILE ${MUPARSER_INCLUDE_DIR}/muParserDef.h)
if (EXISTS ${_VERSION_FILE})
  file (STRINGS ${_VERSION_FILE} _MUPARSER_VERSION_LINE REGEX "#define[ ]+MUP_VERSION[ ]+_T\\(\".*\"\\)")
  if (_MUPARSER_VERSION_LINE)
    string (REGEX REPLACE ".*_T\\(\"(.*)\"\\)" "\\1" MUPARSER_VERSION_STRING ${_MUPARSER_VERSION_LINE})
    if (MUPARSER_VERSION_STRING)
      string (REGEX REPLACE "([0-9]+)\\..*" "\\1" MUPARSER_MAJOR_VERSION ${MUPARSER_VERSION_STRING})
      string (REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" MUPARSER_MINOR_VERSION ${MUPARSER_VERSION_STRING})
      string (REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" MUPARSER_PATCH_VERSION ${MUPARSER_VERSION_STRING})
      if (MUPARSER_PATCH_VERSION STREQUAL MUPARSER_VERSION_STRING)
        set (MUPARSER_PATCH_VERSION)
      endif ()
    endif ()
  endif ()
endif ()

# check version
set (_MUPARSER_VERSION_MATCH TRUE)
if (muParser_FIND_VERSION AND MUPARSER_VERSION_STRING)
  if (muParser_FIND_VERSION_EXACT)
    if (${muParser_FIND_VERSION} VERSION_EQUAL ${MUPARSER_VERSION_STRING})
    else()
      set (_MUPARSER_VERSION_MATCH FALSE)
    endif ()
  else ()
    if (${muParser_FIND_VERSION} VERSION_GREATER ${MUPARSER_VERSION_STRING})
      set (_MUPARSER_VERSION_MATCH FALSE)
    endif ()
  endif ()
endif ()

find_library (MUPARSER_LIBRARY
               NAMES muparser
               PATH_SUFFIXES muparser)

# set MUPARSER_LIBRARIES
set (MUPARSER_LIBRARIES ${MUPARSER_LIBRARY})

# root dir
# try to guess root dir from include dir
if (MUPARSER_INCLUDE_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" MUPARSER_ROOT_DIR ${MUPARSER_INCLUDE_DIR})

# try to guess root dir from library dir
elseif (MUPARSER_LIBRARY)
  string (REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" MUPARSER_ROOT_DIR ${MUPARSER_LIBRARY})
endif ()

# handle REQUIRED and QUIET options
include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (muParser
  REQUIRED_VARS MUPARSER_LIBRARY _MUPARSER_VERSION_MATCH MUPARSER_LIBRARIES MUPARSER_INCLUDE_DIR
  VERSION_VAR MUPARSER_VERSION_STRING
)

mark_as_advanced (
  MUPARSER_LIBRARY
  MUPARSER_LIBRARIES
  MUPARSER_INCLUDE_DIR
  MUPARSER_INCLUDE_DIRS
  MUPARSER_ROOT_DIR 
  MUPARSER_VERSION_STRING
  MUPARSER_MAJOR_VERSION
  MUPARSER_MINOR_VERSION
  MUPARSER_PATCH_VERSION
)
