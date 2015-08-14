#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (SQP.cxx)
ot_add_source_file (SQPSpecificParameters.cxx)

ot_install_header_file (SQP.hxx)
ot_install_header_file (SQPSpecificParameters.hxx)
ot_install_header_file (OTSQP.hxx)
