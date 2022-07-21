# - Find CMinpack
# CMinpack includes software for solving nonlinear equations and nonlinear least squares problems.
# http://devernay.free.fr/hacks/cminpack/
#
# The module defines the following variables:
#  CMINPACK_INCLUDE_DIRS, where to find cminpack.h, etc.
#  CMINPACK_LIBRARIES, the libraries needed to use CMINPACK.
#  CMINPACK_FOUND, If false, do not try to use CMINPACK.
# also defined, but not for general use are
#  CMINPACK_LIBRARY, where to find the CMINPACK library.
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

find_path (CMINPACK_INCLUDE_DIR cminpack.h
  PATH_SUFFIXES cminpack cminpack-1
)

find_library (CMINPACK_LIBRARY
  NAMES cminpack cminpack_s
)

set (CMINPACK_LIBRARIES ${CMINPACK_LIBRARY})
set (CMINPACK_INCLUDE_DIRS ${CMINPACK_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CMinpack DEFAULT_MSG CMINPACK_LIBRARY CMINPACK_INCLUDE_DIRS)

mark_as_advanced (
  CMINPACK_LIBRARY
  CMINPACK_LIBRARIES
  CMINPACK_INCLUDE_DIR
  CMINPACK_INCLUDE_DIRS)

