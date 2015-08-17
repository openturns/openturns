#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (algocobyla.c)
ot_add_source_file (CobylaObsolete.cxx)
ot_add_source_file (CobylaSpecificParameters.cxx)

ot_install_header_file (OTCobyla.hxx)
ot_install_header_file (CobylaObsolete.hxx)
ot_install_header_file (CobylaSpecificParameters.hxx)
