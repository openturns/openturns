#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()
ot_install_header_file (OT.hxx)
ot_add_source_file (openturns_library_ok.c)

# Recurse in subdirectories
add_subdirectory (Base)
add_subdirectory (Uncertainty)

# Build main library
foreach (built_source ${BUILT_SOURCEFILES})
  set_source_files_properties (${built_source} PROPERTIES GENERATED TRUE)
endforeach (built_source)
include_directories (${INTERNAL_INCLUDE_DIRS})
link_directories (${CMAKE_Fortran_IMPLICIT_LINK_DIRECTORIES})
ot_add_library (OT ${SOURCEFILES} ${BUILT_SOURCEFILES})
set_target_properties (OT PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (BUILD_SHARED_LIBS)
  set_target_properties(OT PROPERTIES COMPILE_DEFINITIONS "OT_DLL_EXPORTS")
endif ()

# soversion
if (NOT DEFINED LIB_VERSION) 
  set (LIB_VERSION 0.0.0)
endif ()
if (NOT DEFINED LIB_SOVERSION) 
  set (LIB_SOVERSION 0)
endif ()
set_target_properties (OT PROPERTIES SOVERSION ${LIB_SOVERSION})
set_target_properties (OT PROPERTIES VERSION ${LIB_VERSION})

if (BISON_FOUND AND FLEX_FOUND)
  add_dependencies (OT generate_csv_lexer)
  add_dependencies (OT generate_csv_parser)
endif ()

target_link_libraries (OT ${OPENTURNS_LIBRARIES})
target_link_libraries (OT ${OPENTURNS_PRIVATE_LIBRARIES})

# Build an empty executable to check link dependencies and completeness
add_executable (linktest main.cxx)
target_link_libraries (linktest OT)
set_target_properties (linktest PROPERTIES
                                 INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_BINARY_DIR}/lib/src")

# Add targets to the build-tree export set
export(TARGETS OT FILE "${PROJECT_BINARY_DIR}/OpenTURNS-Targets.cmake")

# Install the export set for use with the install-tree
install(EXPORT OpenTURNS-Targets DESTINATION
  "${OPENTURNS_CONFIG_CMAKE_PATH}"
    COMPONENT    Development)

# Install rules for library and executable
install (TARGETS OT
          EXPORT  OpenTURNS-Targets
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION ${LIBRARY_PATH}
          ARCHIVE DESTINATION ${LIBRARY_PATH}
       )

if (UNIX AND BUILD_SHARED_LIBS)
  set (OT_GDB_PY ${CMAKE_SHARED_LIBRARY_PREFIX}OT${CMAKE_SHARED_LIBRARY_SUFFIX}.${LIB_VERSION}-gdb.py)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/libOT-gdb.py ${CMAKE_CURRENT_BINARY_DIR}/${OT_GDB_PY})
  install (FILES libOT-gdb.py DESTINATION ${OPENTURNS_DATA_PATH}/gdb/auto-load/${LIBRARY_PATH} RENAME ${OT_GDB_PY})
endif ()
