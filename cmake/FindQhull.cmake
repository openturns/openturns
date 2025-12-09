# - Find Qhull
# A general dimension code for computing convex hulls and related structures
# http://www.qhull.org/
#
# The module defines the following variables:
#  QHULL_INCLUDE_DIRS, where to find libqhull_r/qhull_ra.h, etc.
#  QHULL_LIBRARIES, the libraries needed to use Qhull.
#  QHULL_FOUND, If false, do not try to use Qhull.
# also defined, but not for general use are
#  QHULL_LIBRARY, where to find the Qhull library.
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

find_path (QHULL_INCLUDE_DIR libqhull_r/qhull_ra.h)

find_library (QHULL_R_LIBRARY NAMES qhull_r)

set (QHULL_LIBRARIES ${QHULL_R_LIBRARY})
set (QHULL_INCLUDE_DIRS ${QHULL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Qhull DEFAULT_MSG QHULL_R_LIBRARY QHULL_INCLUDE_DIRS)

mark_as_advanced (
  QHULL_R_LIBRARY
  QHULL_LIBRARIES
  QHULL_INCLUDE_DIR
  QHULL_INCLUDE_DIRS)

