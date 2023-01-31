# - Find COIN-OR Optimization Suite
# https://www.coin-or.org/
#
# The module defines the following variables:
#  COIN_FOUND        - True if coin found.
#  COIN_INCLUDE_DIRS - where to find CbcModel.hpp, etc.
#  COIN_<COMPONENT>_LIBRARIES  - Libraries to link for each component.
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
find_path (COIN_INCLUDE_DIR NAMES CbcModel.hpp IpNLP.hpp
  PATH_SUFFIXES coin coin-or
  HINTS "${COIN_ROOT_DIR}/include/coin" "${COIN_ROOT_DIR}/include/coin-or"
)

find_path (COIN_BONMIN_INCLUDE_DIR NAMES BonCbc.hpp
  PATH_SUFFIXES coin coin-or
  HINTS "${COIN_ROOT_DIR}/include/coin" "${COIN_ROOT_DIR}/include/coin-or"
)

if (EXISTS "${COIN_BONMIN_INCLUDE_DIR}/BonminConfig.h")
  file (STRINGS "${COIN_BONMIN_INCLUDE_DIR}/BonminConfig.h" _BONMIN_VERSION_LINE REGEX ".*define[ ]+BONMIN_VERSION[ ].*")
  if (_BONMIN_VERSION_LINE)
    string (REGEX REPLACE ".*BONMIN_VERSION[ ]+\"([0-9\\.]+)\"" "\\1" BONMIN_VERSION_STRING ${_BONMIN_VERSION_LINE})
  endif ()
endif ()

# try to guess root dir from include dir
if (COIN_INCLUDE_DIR AND NOT DEFINED COIN_ROOT_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" COIN_ROOT_DIR ${COIN_INCLUDE_DIR})
endif ()

#
# Find all coin library dependencies
#
find_library (COIN_CBC_LIBRARY
  NAMES Cbc
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_CBC_SOLVER_LIBRARY
  NAMES CbcSolver
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_CGL_LIBRARY
  NAMES Cgl
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_CLP_LIBRARY
  NAMES Clp
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_COIN_UTILS_LIBRARY
  NAMES CoinUtils
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_OSI_LIBRARY
  NAMES Osi
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_OSI_CBC_LIBRARY
  NAMES OsiCbc
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_OSI_CLP_LIBRARY
  NAMES OsiClp
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_ZLIB_LIBRARY
  NAMES z
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_BZ2_LIBRARY
  NAMES bz2
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_IPOPT_LIBRARY
  NAMES ipopt
  HINTS "${COIN_ROOT_DIR}/lib"
)

find_library (COIN_BONMIN_LIBRARY
  NAMES bonmin
  HINTS "${COIN_ROOT_DIR}/lib"
)


#
# Set all required cmake variables based on our findings
#
set (COIN_INCLUDE_DIRS ${COIN_INCLUDE_DIR};${COIN_BONMIN_INCLUDE_DIR})
set (COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARY};${COIN_COIN_UTILS_LIBRARY};${COIN_ZLIB_LIBRARY}")
if (COIN_ZLIB_LIBRARY)
  set (COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARIES};${COIN_ZLIB_LIBRARY}")
endif ()
if (COIN_BZ2_LIBRARY)
  set (COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARIES};${COIN_BZ2_LIBRARY}")
endif ()
set (COIN_CBC_LIBRARIES "${COIN_CBC_LIBRARY};${COIN_CBC_SOLVER_LIBRARY};${COIN_CGL_LIBRARY};${COIN_OSI_LIBRARY};${COIN_OSI_CBC_LIBRARY};${COIN_OSI_CLP_LIBRARY};${COIN_CLP_LIBRARIES}")
set (COIN_CBC_LIBRARIES "${COIN_CBC_LIBRARY};${COIN_CBC_SOLVER_LIBRARY};${COIN_CGL_LIBRARY};${COIN_OSI_LIBRARY};${COIN_OSI_CLP_LIBRARY};${COIN_CLP_LIBRARIES}")
set (COIN_BONMIN_LIBRARIES ${COIN_BONMIN_LIBRARY} ${COIN_IPOPT_LIBRARY} ${COIN_COIN_UTILS_LIBRARY} ${COIN_CBC_LIBRARIES})
set (COIN_IPOPT_LIBRARIES ${COIN_IPOPT_LIBRARY})
set (COIN_LIBRARIES ${COIN_IPOPT_LIBRARIES})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Bonmin
  REQUIRED_VARS COIN_BONMIN_LIBRARY COIN_BONMIN_INCLUDE_DIR COIN_INCLUDE_DIR
  VERSION_VAR BONMIN_VERSION_STRING
)

mark_as_advanced (COIN_INCLUDE_DIR
  COIN_CBC_LIBRARY
  COIN_CBC_SOLVER_LIBRARY
  COIN_CGL_LIBRARY
  COIN_CLP_LIBRARY
  COIN_COIN_UTILS_LIBRARY
  COIN_OSI_LIBRARY
  COIN_OSI_CBC_LIBRARY
  COIN_OSI_CLP_LIBRARY
  COIN_ZLIB_LIBRARY
  COIN_BZ2_LIBRARY
  COIN_BONMIN_LIBRARY
  COIN_IPOPT_LIBRARY)
