#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()
ot_install_header_file (OTUncertainty.hxx)

# Recurse in subdirectories
add_subdirectory (Bayesian)
add_subdirectory (Model)
add_subdirectory (Distribution)
add_subdirectory (Algorithm)
add_subdirectory (StatTests)
add_subdirectory (Process)
