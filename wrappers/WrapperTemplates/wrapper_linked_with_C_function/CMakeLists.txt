cmake_minimum_required (VERSION 2.8)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project (wrapper_linked_with_C_function C)

find_package (OpenTURNS NO_MODULE REQUIRED)
include (${OPENTURNS_USE_FILE})

set (SOURCEFILES myCFunction.c wrapper.c)

add_library (wcode SHARED ${SOURCEFILES})
set_target_properties (wcode PROPERTIES PREFIX "")

set_target_properties (wcode PROPERTIES
                              SUFFIX .so)

target_link_libraries (wcode ${OPENTURNS_WRAPPER_LIBRARIES})

install (TARGETS wcode
          DESTINATION wrappers
       )

install (FILES wcode.xml DESTINATION wrappers)
