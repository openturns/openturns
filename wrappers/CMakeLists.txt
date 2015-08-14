#                                               -*- cmake -*-
        
install (FILES WrapperTemplates/wrapper_calling_shell_command/wrapper.c
                WrapperTemplates/wrapper_calling_shell_command/code_C1.c
                WrapperTemplates/wrapper_calling_shell_command/test.py
                WrapperTemplates/wrapper_calling_shell_command/wcode.xml
                WrapperTemplates/wrapper_calling_shell_command/CMakeLists.txt
                WrapperTemplates/wrapper_calling_shell_command/code_C1.data
          DESTINATION ${OPENTURNS_DATA_PATH}/openturns/WrapperTemplates/wrapper_calling_shell_command
       )
        
install (FILES WrapperTemplates/wrapper_calling_shell_command/customize
          DESTINATION ${OPENTURNS_DATA_PATH}/openturns/WrapperTemplates/wrapper_calling_shell_command
          PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
       )

install (FILES WrapperTemplates/wrapper_linked_with_C_function/wrapper.c
                WrapperTemplates/wrapper_linked_with_C_function/test.py
                WrapperTemplates/wrapper_linked_with_C_function/wcode.xml
                WrapperTemplates/wrapper_linked_with_C_function/myCFunction.h
                WrapperTemplates/wrapper_linked_with_C_function/CMakeLists.txt
                WrapperTemplates/wrapper_linked_with_C_function/myCFunction.c
          DESTINATION ${OPENTURNS_DATA_PATH}/openturns/WrapperTemplates/wrapper_linked_with_C_function
       )
        
install (FILES WrapperTemplates/wrapper_linked_with_C_function/customize
          DESTINATION ${OPENTURNS_DATA_PATH}/openturns/WrapperTemplates/wrapper_linked_with_C_function
          PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
       )

install (FILES WrapperTemplates/wrapper_linked_with_F77_function/wrapper.c
                WrapperTemplates/wrapper_linked_with_F77_function/test.py
                WrapperTemplates/wrapper_linked_with_F77_function/wcode.xml
                WrapperTemplates/wrapper_linked_with_F77_function/code.f
                WrapperTemplates/wrapper_linked_with_F77_function/CMakeLists.txt
          DESTINATION ${OPENTURNS_DATA_PATH}/openturns/WrapperTemplates/wrapper_linked_with_F77_function
       )
        
install (FILES WrapperTemplates/wrapper_linked_with_F77_function/customize
          DESTINATION ${OPENTURNS_DATA_PATH}/openturns/WrapperTemplates/wrapper_linked_with_F77_function
          PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
       )

