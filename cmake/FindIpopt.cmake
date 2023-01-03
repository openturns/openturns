# - Find Ipopt solver
# https://www.coin-or.org/
#
# The module defines the following variables:
#  IPOPT_FOUND        - True if coin found.
#  IPOPT_INCLUDE_DIRS - where to find CbcModel.hpp, etc.
#  IPOPT_LIBRARIES    - Libraries to link for each component.
#  IPOPT_VERSION
#  IPOPT_DEFINITIONS
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
#
# Find the path based on a required header file
#

find_package(PkgConfig QUIET)

# Check to see if pkgconfig is installed.
pkg_check_modules (PC_IPOPT ipopt QUIET)

# Definitions
set (IPOPT_DEFINITIONS ${PC_IPOPT_CFLAGS_OTHER})

# Include directories
find_path (IPOPT_INCLUDE_DIR
  NAMES IpIpoptNLP.hpp
  PATH_SUFFIXES coin-or coin
  HINTS ${PC_IPOPT_INCLUDEDIR}
)

set (IPOPT_INCLUDE_DIRS ${IPOPT_INCLUDE_DIR})

# Libraries
find_library (IPOPT_LIBRARY
  NAMES ipopt
  HINTS ${PC_IPOPT_LIBDIR}
)

set (IPOPT_LIBRARIES ${IPOPT_LIBRARY})

# Version
set(IPOPT_VERSION ${PC_IPOPT_VERSION})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Ipopt
    REQUIRED_VARS IPOPT_LIBRARIES IPOPT_INCLUDE_DIRS
    VERSION_VAR   IPOPT_VERSION)
