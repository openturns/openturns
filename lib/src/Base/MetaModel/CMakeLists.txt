#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (LinearTaylor.cxx)
ot_add_source_file (QuadraticTaylor.cxx)
ot_add_source_file (LinearLeastSquares.cxx)
ot_add_source_file (QuadraticLeastSquares.cxx)

ot_install_header_file (OTBaseMetaModel.hxx)
ot_install_header_file (LinearTaylor.hxx)
ot_install_header_file (QuadraticTaylor.hxx)
ot_install_header_file (LinearLeastSquares.hxx)
ot_install_header_file (QuadraticLeastSquares.hxx)
