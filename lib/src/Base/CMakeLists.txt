#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()
ot_install_header_file (OTBase.hxx)

# Recurse in subdirectories
add_subdirectory (Algo)
add_subdirectory (Common)
add_subdirectory (Diff)
add_subdirectory (Experiments)
add_subdirectory (Func)
add_subdirectory (Geom)
add_subdirectory (Graph)
add_subdirectory (MetaModel)
add_subdirectory (Optim)
add_subdirectory (Solver)
add_subdirectory (Stat)
add_subdirectory (Type)
add_subdirectory (IterativeStat)
