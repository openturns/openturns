#                                               -*- cmake -*-

ot_add_current_dir_to_include_dirs ()
ot_add_build_dir_to_include_dirs (${CMAKE_CURRENT_BINARY_DIR})

string (REGEX REPLACE "(dev|rc[0-9])" "" _VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
math (EXPR OPENTURNS_VERSION_NR "100000 * ${CPACK_PACKAGE_VERSION_MAJOR} + 100 * ${_VERSION_MINOR}")
if (CPACK_PACKAGE_VERSION_PATCH)
  math (EXPR OPENTURNS_VERSION_NR "${OPENTURNS_VERSION_NR} + ${CPACK_PACKAGE_VERSION_PATCH}")
endif ()


configure_file (${CMAKE_CURRENT_SOURCE_DIR}/OTconfig.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/openturns/OTconfig.h
                @ONLY)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/installed-OTconfig.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/openturns/installed-OTconfig.h
                @ONLY)

if (NOT ot_configure_date)
  if (UNIX)
    execute_process (COMMAND date "+%a, %d %b %Y %H:%M:%S %z"
                      OUTPUT_VARIABLE ot_configure_date
                      OUTPUT_STRIP_TRAILING_WHITESPACE
                   )
  elseif(MSVC OR WINTEL)
    execute_process (COMMAND cmd " /C date /T"
                      OUTPUT_VARIABLE ot_configure_date
                      OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
  set (ot_configure_date "${ot_configure_date}" CACHE STRING "First configure date.")
endif ()
if (NOT ot_revision)
  execute_process (COMMAND git rev-parse --short HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE ot_revision
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                 )
  set (ot_revision "${ot_revision}" CACHE STRING "Revision number.")
endif ()
mark_as_advanced(ot_configure_date ot_revision)
set (prefix ${CMAKE_INSTALL_PREFIX})
set (PACKAGE openturns)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OTconfigure_args.hxx.in
                ${CMAKE_CURRENT_BINARY_DIR}/openturns/OTconfigureArgs.hxx
                @ONLY
             )

# with in-source build, installed-OTconfig.h must overwrite the internal openturns/OTconfig.h
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/openturns/installed-OTconfig.h
          DESTINATION ${OPENTURNS_INCLUDE_PATH}/openturns
          RENAME OTconfig.h
       )
install (DIRECTORY openturns DESTINATION ${OPENTURNS_INCLUDE_PATH})

if (MSVC OR WINTEL)
    install (FILES pthread_win32/pthread.h
                    pthread_win32/semaphore.h
                    pthread_win32/unistd.h
              DESTINATION ${OPENTURNS_INCLUDE_PATH}
           )
endif ()
