cmake_minimum_required (VERSION 2.8)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project (wrapper_linked_with_F77_function C Fortran)

# workaround for bug #0014358
if (CMAKE_VERSION VERSION_LESS 2.8.13 AND NOT DEFINED FortranCInterface_EXE)
  set (FortranCInterface_EXE ${CMAKE_BINARY_DIR}/CMakeFiles/FortranCInterface/FortranCInterface${CMAKE_EXECUTABLE_SUFFIX})
endif ()

include (FortranCInterface)
FortranCInterface_HEADER (FortranCInterface.h)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

find_package (OpenTURNS NO_MODULE REQUIRED)
include (${OPENTURNS_USE_FILE})

set (SOURCEFILES code.f wrapper.c)

add_library (wcode SHARED ${SOURCEFILES})
set_target_properties (wcode PROPERTIES PREFIX "")

set_target_properties (wcode PROPERTIES
                              SUFFIX .so) 
target_link_libraries (wcode ${OPENTURNS_WRAPPER_LIBRARIES})

install (TARGETS wcode
          DESTINATION wrappers
       )

install (FILES wcode.xml DESTINATION wrappers)
