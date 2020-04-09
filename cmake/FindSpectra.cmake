# Find Spectra
# A header-only C++ library for large scale eigenvalue problems.
# https://spectralib.org/
#
# The module defines the following variables:
#  SPECTRA_INCLUDE_DIRS, where to find Spectra/GenEigsSolver.h, etc.
#
#=============================================================================
# Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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


find_path (SPECTRA_INCLUDE_DIR 
  NAMES Spectra/GenEigsSolver.h
  HINTS "${SPECTRA_ROOT_DIR}/include"
)

find_package (Eigen3 NO_MODULE)

# workaround broken eigen3 cmake config
if (EIGEN3_INCLUDE_DIRS AND NOT EXISTS "${EIGEN3_INCLUDE_DIRS}")
  unset (EIGEN3_INCLUDE_DIRS)
  find_path (EIGEN3_INCLUDE_DIRS NAMES signature_of_eigen3_matrix_library
              PATH_SUFFIXES eigen3
              HINTS ${EIGEN3_ROOT_DIR}/include)
endif ()

set (SPECTRA_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIRS} ${SPECTRA_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Spectra DEFAULT_MSG SPECTRA_INCLUDE_DIRS EIGEN3_INCLUDE_DIRS)
