# - Find MPFR
# MPFR includes software for computing with arbitrary precision floating points
# https://www.mpfr.org/
#
# The module defines the following variables:
#  MPFR_INCLUDE_DIRS, where to find mpfr.h, etc.
#  MPFR_LIBRARIES, the libraries needed to use MPFR.
#  MPFR_FOUND, If false, do not try to use MPFR.
# also defined, but not for general use are
#  MPFR_LIBRARY, where to find the MPFR library.
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

find_path (MPFR_INCLUDE_DIR mpfr.h
  PATH_SUFFIXES mpfr
)

find_library (MPFR_LIBRARY
  NAMES mpfr
)

set (MPFR_LIBRARIES ${MPFR_LIBRARY})
set (MPFR_INCLUDE_DIRS ${MPFR_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_LIBRARY MPFR_INCLUDE_DIRS)

mark_as_advanced (
  MPFR_LIBRARY
  MPFR_LIBRARIES
  MPFR_INCLUDE_DIR
  MPFR_INCLUDE_DIRS)

