#                                               -*- cmake -*-

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/openturns.conf.in
                 ${CMAKE_CURRENT_BINARY_DIR}/openturns.conf
                 @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/openturns.conf
          DESTINATION ${OPENTURNS_SYSCONFIG_PATH}/openturns
       )
