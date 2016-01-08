#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (algotnc.c)
ot_add_source_file (TNC.cxx)

ot_install_header_file (OTTNC.hxx)
ot_install_header_file (TNC.hxx)
