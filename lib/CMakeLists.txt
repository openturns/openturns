#                                               -*- cmake -*-

# Recurse in subdirectories
add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (etc)

# Build the OpenTURNSConfig.cmake file
set (OPENTURNS_LIBRARY_DIRS            ${LIBRARY_PATH})
set (OPENTURNS_LIBRARY                 OT)
list (APPEND OPENTURNS_LIBRARIES       ${OPENTURNS_LIBRARY})
set (OPENTURNS_INCLUDE_DIR             ${INCLUDE_PATH})
list (INSERT OPENTURNS_INCLUDE_DIRS 0  ${OPENTURNS_INCLUDE_DIR})

set (OPENTURNS_ROOT_DIR       ${INSTALL_PATH})
set (OPENTURNS_VERSION_STRING ${PACKAGE_VERSION})
set (OPENTURNS_VERSION_MAJOR  ${CPACK_PACKAGE_VERSION_MAJOR})
set (OPENTURNS_VERSION_MINOR  ${CPACK_PACKAGE_VERSION_MINOR})
set (OPENTURNS_VERSION_PATCH  ${CPACK_PACKAGE_VERSION_PATCH})

include(CMakePackageConfigHelpers)
configure_package_config_file (OpenTURNSConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/OpenTURNSConfig.cmake
  INSTALL_DESTINATION ${OPENTURNS_CONFIG_CMAKE_PATH}
  PATH_VARS
    OPENTURNS_ROOT_DIR
    OPENTURNS_INCLUDE_DIR
    OPENTURNS_INCLUDE_DIRS
    OPENTURNS_LIBRARY_DIRS
    OPENTURNS_SWIG_INCLUDE_DIRS
    PYTHON_MODULE_PATH
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(OpenTURNSConfigVersion.cmake VERSION ${OPENTURNS_VERSION_STRING} COMPATIBILITY AnyNewerVersion)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenTURNSConfig.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/OpenTURNSConfigVersion.cmake
         DESTINATION ${OPENTURNS_CONFIG_CMAKE_PATH})

# Recurse in subdirectories : test should be run after src (for target OT to be built)
add_subdirectory (test)

install (FILES ${HEADERFILES} DESTINATION ${OPENTURNS_INCLUDE_PATH}/openturns)
