
find_package (OpenTURNS NO_MODULE REQUIRED)

set (SOURCEFILES AnalyticalFunctionCompiler.cxx CreateLibrary.cxx  CreateSourceCode.cxx  CreateWrapper.cxx  Main.cxx)

add_definitions (-DOT_INCLUDE_PATH=\"/usr/include/openturns\")
add_definitions (-DOT_LIB_PATH=\"/usr/lib\")
add_definitions (-DOT_LIB_NAME=\"libOT.so\")

add_executable (afc ${SOURCEFILES})

install (TARGETS afc
          DESTINATION bin
       )
        

