# - Find R
# R is a free software environment for statistical computing and graphics
# http://www.r-project.org/
#
# The module defines the following variables:
#  R_FOUND - System has R
#  R_EXECUTABLE     - The R interpreter
#  R_INCLUDE_DIR    - the R include directory
#  R_LIBRARIES      - The libraries needed to use R
#  R_VERSION_STRING - R version
#
#=============================================================================
# Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

if (R_EXECUTABLE)
 # in cache already
 set(R_FIND_QUIETLY TRUE)
endif ()

find_program (R_EXECUTABLE
               NAMES R R.exe
               DOC "Path to the R command interpreter"
             )

if (R_EXECUTABLE)
  execute_process (COMMAND ${R_EXECUTABLE} --slave --no-save -e "cat(R.home('home'))"
                    OUTPUT_VARIABLE _R_HOME
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                 )
                  
  execute_process (COMMAND ${R_EXECUTABLE} --slave --no-save -e "cat(R.home('include'))"
                    OUTPUT_VARIABLE _R_INCLUDE
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                 )
  execute_process (COMMAND ${R_EXECUTABLE} --slave --no-save -e "cat(R.version.string)"
                    OUTPUT_VARIABLE R_VERSION_STRING
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                 )     
  if (R_VERSION_STRING)
    string (REGEX REPLACE ".*version (.*) .*" "\\1" R_VERSION_STRING ${R_VERSION_STRING})
  endif ()                                
endif ()

find_path (R_INCLUDE_DIR
            NAMES R.h
            HINTS
            ${_R_INCLUDE}
         )

find_library (R_LIBRARIES
  NAMES R
  HINTS
  ${PC_R_LIBDIR}
  ${PC_R_LIBRARY_DIRS}
  ${_R_HOME}/lib
  ${_R_HOME}/lib/x86_64
)

set (R_PACKAGES)
if (R_EXECUTABLE)
  foreach (_component ${R_FIND_COMPONENTS})
    if (NOT R_${_component}_FOUND)
      execute_process (COMMAND echo "library(${_component})"
                        COMMAND ${R_EXECUTABLE} --no-save --silent --no-readline --slave
                        RESULT_VARIABLE _res
                        OUTPUT_VARIABLE _trashout
                        ERROR_VARIABLE  _trasherr
                     )
      if (NOT _res)
        message (STATUS "Looking for R package ${_component} - found")
        set (R_${_component}_FOUND 1 CACHE INTERNAL "True if R package ${_component} is here")
      else ()
        message (STATUS "Looking for R package ${_component} - not found")
        set (R_${_component}_FOUND 0 CACHE INTERNAL "True if R package ${_component} is here")
      endif ()
# find_package's OPTIONAL_COMPONENTS is only available as of cmake 2.8.8
#       if (R_FIND_REQUIRED_${_component})
#         list (APPEND R_PACKAGES R_${_component}_FOUND)
#       endif ()
    endif ()
  endforeach ()
endif ()

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (R
  REQUIRED_VARS R_EXECUTABLE ${R_PACKAGES}
  VERSION_VAR R_VERSION_STRING
)

mark_as_advanced (R_EXECUTABLE R_INCLUDE_DIR R_LIBRARIES R_VERSION_STRING ${R_PACKAGES})
