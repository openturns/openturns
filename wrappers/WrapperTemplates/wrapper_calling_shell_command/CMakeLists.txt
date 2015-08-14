cmake_minimum_required (VERSION 2.8)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project (wrapper_calling_shell_command C)

find_package (OpenTURNS NO_MODULE REQUIRED)
include (${OPENTURNS_USE_FILE})

set (SOURCEFILES wrapper.c)

add_library (wcode SHARED ${SOURCEFILES})
set_target_properties (wcode PROPERTIES PREFIX "")

set_target_properties (wcode PROPERTIES
                              SUFFIX .so) 
target_link_libraries (wcode ${OPENTURNS_WRAPPER_LIBRARIES})

install (TARGETS wcode
          DESTINATION wrappers
       )

install (FILES wcode.xml DESTINATION wrappers)

add_executable(code_C1 code_C1.c)
target_link_libraries (code_C1 ${REGEX_LIBRARIES})

if (UNIX)
  target_link_libraries (code_C1 m)
endif ()

install (TARGETS code_C1
          DESTINATION bin
       )
