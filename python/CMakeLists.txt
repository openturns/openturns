#                                               -*- cmake -*-

include_directories (${PYTHON_INCLUDE_DIRS})

# Recurse in subdirectories
add_subdirectory (src)
add_subdirectory (test)

