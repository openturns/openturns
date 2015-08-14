#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (RosenblattEvaluation.cxx)
ot_add_source_file (InverseRosenblattEvaluation.cxx)

ot_install_header_file (OTRosenblatt.hxx)
ot_install_header_file (InverseRosenblattEvaluation.hxx)
ot_install_header_file (RosenblattEvaluation.hxx)
