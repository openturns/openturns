# - Find MPC
# MPC includes software for computing with multiprecision complex numbers
# http://www.multiprecision.org/mpc
#
# The module defines the following variables:
#  MPC_INCLUDE_DIRS, where to find mpc.h, etc.
#  MPC_LIBRARIES, the libraries needed to use MPC.
#  MPC_FOUND, If false, do not try to use MPC.
# also defined, but not for general use are
#  MPC_LIBRARY, where to find the MPC library.
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

find_path (MPC_INCLUDE_DIR mpc.h
  PATH_SUFFIXES mpc
)

find_library (MPC_LIBRARY
  NAMES mpc
)

set (MPC_LIBRARIES ${MPC_LIBRARY})
set (MPC_INCLUDE_DIRS ${MPC_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPC DEFAULT_MSG MPC_LIBRARY MPC_INCLUDE_DIRS)

mark_as_advanced (
  MPC_LIBRARY
  MPC_LIBRARIES
  MPC_INCLUDE_DIR
  MPC_INCLUDE_DIRS)

