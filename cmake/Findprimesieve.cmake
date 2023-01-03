# - Find primesieve
# Fast prime number generator
# https://primesieve.org
#
# The module defines the following variables:
#  PRIMESIEVE_INCLUDE_DIRS, where to find mpc.h, etc.
#  PRIMESIEVE_LIBRARIES, the libraries needed to use primesieve.
#  PRIMESIEVE_FOUND, If false, do not try to use primesieve.
# also defined, but not for general use are
#  PRIMESIEVE_LIBRARY, where to find the primesieve library.
#
#=============================================================================
# Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

find_path (PRIMESIEVE_INCLUDE_DIR primesieve.h)

if (EXISTS ${PRIMESIEVE_INCLUDE_DIR}/primesieve.h)
  file (STRINGS ${PRIMESIEVE_INCLUDE_DIR}/primesieve.h _VERSION_STRING REGEX ".*define[ ]+PRIMESIEVE_VERSION[ ]+.*")
  if (_VERSION_STRING)
    string (REGEX REPLACE ".*PRIMESIEVE_VERSION[ ]+\"(.*)\"" "\\1" primesieve_VERSION ${_VERSION_STRING})
  endif ()
endif ()

find_library (PRIMESIEVE_LIBRARY NAMES primesieve)

set (PRIMESIEVE_LIBRARIES ${PRIMESIEVE_LIBRARY})
set (PRIMESIEVE_INCLUDE_DIRS ${PRIMESIEVE_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(primesieve VERSION_VAR primesieve_VERSION REQUIRED_VARS PRIMESIEVE_LIBRARIES PRIMESIEVE_INCLUDE_DIRS)

mark_as_advanced (
  PRIMESIEVE_LIBRARY
  PRIMESIEVE_LIBRARIES
  PRIMESIEVE_INCLUDE_DIR
  PRIMESIEVE_INCLUDE_DIRS)

