#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (EfficientGlobalOptimization.cxx)

ot_install_header_file (EfficientGlobalOptimization.hxx)

ot_install_header_file (OTUncertaintyOptimization.hxx)

