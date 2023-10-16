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
if (Ceres_FOUND)
  set_source_files_properties (Base/Optim/Ceres.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Exclude Dlib sources from unity build (Lapack re-included)
if (dlib_FOUND)
  set_source_files_properties (Base/Optim/Dlib.cxx Base/Optim/DlibFunctions.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Exclude Ipopt sources
if (Ipopt_FOUND)
  set_source_files_properties (Base/Optim/Ipopt.cxx Base/Optim/IpoptProblem.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Exclude Bonmin sources
if (Bonmin_FOUND)
  set_source_files_properties (Base/Optim/Bonmin.cxx Base/Optim/BonminProblem.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
      set_source_files_properties (Base/Optim/Bonmin.cxx Base/Optim/BonminProblem.cxx PROPERTIES COMPILE_FLAGS "-Wno-error=unused-parameter")
    else ()
      set_source_files_properties (Base/Optim/Bonmin.cxx Base/Optim/BonminProblem.cxx PROPERTIES COMPILE_FLAGS "-Wno-error=unused-parameter -Wno-error=implicit-fallthrough")
    endif ()
  endif ()
endif ()

if (Pagmo_FOUND)
  set_source_files_properties (Base/Optim/Pagmo.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Exclude LibXML sources
if (LIBXML2_FOUND)
  set_source_files_properties (Base/Common/XMLStorageManager.cxx Base/Common/XMLToolbox.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Exclude HDF5 sources
if (LIBXML2_FOUND AND HDF5_FOUND)
  set_source_files_properties (Base/Common/XMLH5StorageManager.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Exclude HMAT sources
if (HMAT_FOUND)
  set_source_files_properties (Base/Stat/HMatrixFactory.cxx Base/Stat/HMatrixImplementation.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# SymbolicParserExprTk.cxx includes exprtk.hpp which is huge, exclude it from unity builds
# It also causes problems on Windows
if (OPENTURNS_HAVE_EXPRTK)
  set_source_files_properties ("Base/Func/SymbolicParserExprTk.cxx" PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
  if (MINGW)
    set_source_files_properties ("Base/Func/SymbolicParserExprTk.cxx" PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj -g0")
  endif ()
endif()

# Exclude spectra sources from unity build (templates take a lot of memory)
if (Spectra_FOUND)
  set_source_files_properties (Base/Algo/KarhunenLoeveP1Algorithm.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

# Build main library
foreach (built_source ${BUILT_SOURCEFILES})
  set_source_files_properties (${built_source} PROPERTIES GENERATED TRUE)
endforeach (built_source)
include_directories (BEFORE ${INTERNAL_INCLUDE_DIRS})
add_library (OT ${SOURCEFILES} ${BUILT_SOURCEFILES})
set_target_properties (OT PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (BUILD_SHARED_LIBS)
  set_target_properties(OT PROPERTIES COMPILE_DEFINITIONS "OT_DLL_EXPORTS")
endif ()

# soversion
set_target_properties (OT PROPERTIES SOVERSION 0.23)
set (LIB_VERSION 0.23.0)
set_target_properties (OT PROPERTIES VERSION ${LIB_VERSION})

if (BISON_FOUND AND FLEX_FOUND)
  add_dependencies (OT generate_csv_lexer)
  add_dependencies (OT generate_csv_parser)
endif ()

target_link_libraries (OT PUBLIC ${OPENTURNS_LIBRARIES})
target_link_libraries (OT PRIVATE ${OPENTURNS_PRIVATE_LIBRARIES})

target_include_directories (OT INTERFACE $<INSTALL_INTERFACE:include>)
target_include_directories (OT PUBLIC ${OPENTURNS_INCLUDE_DIRS})

target_compile_features(OT PUBLIC cxx_std_11)

set_property (TARGET OT PROPERTY OPENTURNS_ENABLED_FEATURES "${OPENTURNS_ENABLED_FEATURES}")
set_property (TARGET OT APPEND PROPERTY EXPORT_PROPERTIES "OPENTURNS_ENABLED_FEATURES")

# Build an empty executable to check link dependencies and completeness
add_executable (linktest main.cxx)
target_link_libraries (linktest OT)
set_target_properties (linktest PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${PROJECT_BINARY_DIR}/lib/src")

# Add targets to the build-tree export set
export(TARGETS OT FILE "${PROJECT_BINARY_DIR}/OpenTURNS-Targets.cmake")

# Install the export set for use with the install-tree
install(EXPORT OpenTURNS-Targets DESTINATION ${OPENTURNS_CONFIG_CMAKE_PATH} COMPONENT Development)

# Install rules for library and executable
install (TARGETS OT
          EXPORT  OpenTURNS-Targets
          RUNTIME DESTINATION ${OPENTURNS_BIN_PATH}
          LIBRARY DESTINATION ${OPENTURNS_LIBRARY_PATH}
          ARCHIVE DESTINATION ${OPENTURNS_LIBRARY_PATH})

if (UNIX AND BUILD_SHARED_LIBS)
  set (OT_GDB_PY ${CMAKE_SHARED_LIBRARY_PREFIX}OT${CMAKE_SHARED_LIBRARY_SUFFIX}.${LIB_VERSION}-gdb.py)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/libOT-gdb.py ${CMAKE_CURRENT_BINARY_DIR}/${OT_GDB_PY})
  install (FILES libOT-gdb.py DESTINATION ${OPENTURNS_DATA_PATH}/gdb/auto-load/${LIBRARY_PATH} RENAME ${OT_GDB_PY})
endif ()
