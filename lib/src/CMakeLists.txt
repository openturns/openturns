#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()
ot_install_header_file (OT.hxx)
ot_add_source_file (openturns_library_ok.c)

# Recurse in subdirectories
add_subdirectory (Base)
add_subdirectory (Uncertainty)

set_source_files_properties (Base/Func/SpecFunc/Faddeeva.cc PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

# Exclude Ceres sources from unity build (templates take much memory)
set_source_files_properties (Base/Optim/Ceres.cxx PROPERTIES COTIRE_EXCLUDED ON SKIP_UNITY_BUILD_INCLUSION ON)

# Exclude Dlib sources from unity build (Lapack re-included)
set_source_files_properties (Base/Optim/Dlib.cxx Base/Optim/DlibFunctions.cxx PROPERTIES COTIRE_EXCLUDED ON SKIP_UNITY_BUILD_INCLUSION ON)

# Exclude Eigen3 sources from unity build
set_source_files_properties (Base/Type/SparseMatrix.cxx PROPERTIES COTIRE_EXCLUDED ON SKIP_UNITY_BUILD_INCLUSION ON)

# Exclude Bonmin/Ipopt sources
set_source_files_properties (Base/Optim/Bonmin.cxx Base/Optim/BonminProblem.cxx Base/Optim/Ipopt.cxx Base/Optim/IpoptProblem.cxx PROPERTIES COTIRE_EXCLUDED ON SKIP_UNITY_BUILD_INCLUSION ON)

# SymbolicParserExprTk.cxx includes exprtk.hpp which is huge, exclude it from cotire builds
# It also causes problems on Windows
if (OPENTURNS_HAVE_EXPRTK)
  set_source_files_properties ("Base/Func/SymbolicParserExprTk.cxx" PROPERTIES COTIRE_EXCLUDED ON SKIP_UNITY_BUILD_INCLUSION ON)
  if (MINGW)
    set_source_files_properties ("Base/Func/SymbolicParserExprTk.cxx" PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj -g0")
  elseif (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "MSVC|Intel")
    set_source_files_properties ("Base/Func/SymbolicParserExprTk.cxx" PROPERTIES COMPILE_FLAGS "/bigobj")
  endif ()
endif()

set_source_files_properties (Base/Optim/Ipopt.cxx Base/Optim/IpoptProblem.cxx PROPERTIES COMPILE_DEFINITIONS "HAVE_STDDEF_H") # required for ipopt 3.11
if ((OPENTURNS_HAVE_BONMIN OR OPENTURNS_HAVE_IPOPT) AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
    set_source_files_properties (Base/Optim/Bonmin.cxx Base/Optim/BonminProblem.cxx Base/Optim/Ipopt.cxx Base/Optim/IpoptProblem.cxx PROPERTIES COMPILE_FLAGS "-Wno-error=unused-parameter")
  else ()
    set_source_files_properties (Base/Optim/Bonmin.cxx Base/Optim/BonminProblem.cxx Base/Optim/Ipopt.cxx Base/Optim/IpoptProblem.cxx PROPERTIES COMPILE_FLAGS "-Wno-error=unused-parameter -Wno-error=implicit-fallthrough")
  endif ()
endif ()

# Build main library
foreach (built_source ${BUILT_SOURCEFILES})
  set_source_files_properties (${built_source} PROPERTIES GENERATED TRUE)
endforeach (built_source)
include_directories (BEFORE ${INTERNAL_INCLUDE_DIRS})
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

if (CMAKE_VERSION VERSION_LESS 3)
  target_link_libraries (OT ${OPENTURNS_LIBRARIES})
  target_link_libraries (OT ${OPENTURNS_PRIVATE_LIBRARIES})
  set_target_properties (OT PROPERTIES INTERFACE_LINK_LIBRARIES "${OPENTURNS_LIBRARIES}")
else ()
  target_link_libraries (OT PUBLIC ${OPENTURNS_LIBRARIES})
  target_link_libraries (OT PRIVATE ${OPENTURNS_PRIVATE_LIBRARIES})
endif ()

if (NOT CMAKE_VERSION VERSION_LESS 3.2)
  # Else Debian packages of modules fail to build as destination is absolute
  # https://gitlab.kitware.com/cmake/cmake/commit/dd089e08b578f20b7dc7d2ce658e3df05e346e35
  target_include_directories (OT INTERFACE $<INSTALL_INTERFACE:include>)
  target_include_directories (OT PUBLIC ${OPENTURNS_INCLUDE_DIRS})
endif ()

# Build an empty executable to check link dependencies and completeness
add_executable (linktest main.cxx)
target_link_libraries (linktest OT)
set_target_properties (linktest PROPERTIES
                                 INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_BINARY_DIR}/lib/src")

# Add targets to the build-tree export set
export(TARGETS OT FILE "${PROJECT_BINARY_DIR}/OpenTURNS-Targets.cmake")

# Install the export set for use with the install-tree
install(EXPORT OpenTURNS-Targets DESTINATION ${OPENTURNS_CONFIG_CMAKE_PATH} COMPONENT Development)

# Install rules for library and executable
install (TARGETS OT
          EXPORT  OpenTURNS-Targets
          RUNTIME DESTINATION bin
	  LIBRARY DESTINATION ${OPENTURNS_LIBRARY_PATH}
	  ARCHIVE DESTINATION ${OPENTURNS_LIBRARY_PATH}
       )

if (UNIX AND BUILD_SHARED_LIBS)
  set (OT_GDB_PY ${CMAKE_SHARED_LIBRARY_PREFIX}OT${CMAKE_SHARED_LIBRARY_SUFFIX}.${LIB_VERSION}-gdb.py)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/libOT-gdb.py ${CMAKE_CURRENT_BINARY_DIR}/${OT_GDB_PY})
  install (FILES libOT-gdb.py DESTINATION ${OPENTURNS_DATA_PATH}/gdb/auto-load/${LIBRARY_PATH} RENAME ${OT_GDB_PY})
endif ()
