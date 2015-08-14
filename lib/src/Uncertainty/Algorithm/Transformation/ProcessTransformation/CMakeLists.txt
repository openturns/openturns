#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (BoxCoxFactory.cxx)
ot_add_source_file (TrendFactory.cxx)

ot_install_header_file (BoxCoxFactory.hxx)
ot_install_header_file (TrendFactory.hxx)
ot_install_header_file (OTProcessTransformation.hxx)
