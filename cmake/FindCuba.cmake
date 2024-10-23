# - Find Cuba
# Cuba includes software for multidimensional numerical integration
# https://feynarts.de/cuba/
#
# The module defines the following variables:
#  CUBA_INCLUDE_DIRS, where to find Cuba.h, etc.
#  CUBA_LIBRARIES, the libraries needed to use Cuba.
#  CUBA_FOUND, If false, do not try to use Cuba.
# also defined, but not for general use are
#  CUBA_LIBRARY, where to find the Cuba library.
#
#=============================================================================
# Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

find_path (CUBA_INCLUDE_DIR cuba.h
  PATH_SUFFIXES cuba
)

find_library (CUBA_LIBRARY
  NAMES cuba
)

set (CUBA_LIBRARIES ${CUBA_LIBRARY})
set (CUBA_INCLUDE_DIRS ${CUBA_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cuba DEFAULT_MSG CUBA_LIBRARY CUBA_INCLUDE_DIRS)

mark_as_advanced (
  CUBA_LIBRARY
  CUBA_LIBRARIES
  CUBA_INCLUDE_DIR
  CUBA_INCLUDE_DIRS)

