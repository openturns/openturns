#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (algotnc.c)
ot_add_source_file (TNCObsolete.cxx)
ot_add_source_file (TNCSpecificParameters.cxx)
ot_add_source_file (TNC.cxx)

ot_install_header_file (OTTNC.hxx)
ot_install_header_file (TNCObsolete.hxx)
ot_install_header_file (TNCSpecificParameters.hxx)
ot_install_header_file (TNC.hxx)
