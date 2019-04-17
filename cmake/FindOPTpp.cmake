# - Find OPT++
# An Object-Oriented Nonlinear Optimization Library
# https://software.sandia.gov/opt++/
#
# The module defines the following variables:
#  OPTPP_FOUND - the system has opt++
#  OPTPP_INCLUDE_DIR - where to find Opt.h
#  OPTPP_INCLUDE_DIRS - opt++ includes
#  OPTPP_LIBRARY - where to find the opt++ library
#  OPTPP_LIBRARIES - aditional libraries
#  OPTPP_ROOT_DIR - root dir (ex. /usr/local)

# set OPTPP_INCLUDE_DIR
find_path (OPTPP_INCLUDE_DIR
  NAMES Opt.h
  PATH_SUFFIXES opt++
  DOC
    "OPT++ include directory"
)

# set OPTPP_INCLUDE_DIRS
set (OPTPP_INCLUDE_DIRS ${OPTPP_INCLUDE_DIR})

# set OPTPP_LIBRARY
find_library (OPTPP_LIBRARY
  NAMES opt
  DOC
    "OPT++ library location"
)

find_library (OPTPP_NEWMAT_LIBRARY
  NAMES newmat
  DOC
    "OPT++ library location"
)

# set OPTPP_LIBRARIES
set (OPTPP_LIBRARIES ${OPTPP_LIBRARY} ${OPTPP_NEWMAT_LIBRARY} ${BLAS_LIBRARIES})

# root dir
# try to guess root dir from include dir
if (OPTPP_INCLUDE_DIR)
  string (REGEX REPLACE "(.*)/include.*" "\\1" OPTPP_ROOT_DIR ${OPTPP_INCLUDE_DIR})

# try to guess root dir from library dir
elseif (OPTPP_LIBRARY)
  string (REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" OPTPP_ROOT_DIR ${OPTPP_LIBRARY})
endif ()

# handle REQUIRED and QUIET options
include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (OPTpp DEFAULT_MSG OPTPP_LIBRARY
  OPTPP_INCLUDE_DIR
  OPTPP_INCLUDE_DIRS
  OPTPP_LIBRARIES
  OPTPP_ROOT_DIR
)


mark_as_advanced (
  OPTPP_LIBRARY
  OPTPP_NEWMAT_LIBRARY
  OPTPP_LIBRARIES
  OPTPP_INCLUDE_DIR
  OPTPP_INCLUDE_DIRS
  OPTPP_ROOT_DIR
)
