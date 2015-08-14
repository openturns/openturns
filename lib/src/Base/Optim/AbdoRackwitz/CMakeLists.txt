#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (AbdoRackwitz.cxx)
ot_add_source_file (AbdoRackwitzSpecificParameters.cxx)

ot_install_header_file (AbdoRackwitz.hxx)
ot_install_header_file (AbdoRackwitzSpecificParameters.hxx)
ot_install_header_file (OTAbdoRackwitz.hxx)
