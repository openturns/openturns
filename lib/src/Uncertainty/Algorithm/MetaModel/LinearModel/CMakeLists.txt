#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()


ot_add_source_file (LinearModelResult.cxx)
ot_add_source_file (LinearModelAlgorithm.cxx)
ot_add_source_file (LinearModelAnalysis.cxx)
ot_add_source_file (LinearModel.cxx)
ot_add_source_file (LinearModelFactory.cxx)

ot_install_header_file (LinearModelResult.hxx)
ot_install_header_file (LinearModelAlgorithm.hxx)
ot_install_header_file (LinearModelAnalysis.hxx)
ot_install_header_file (LinearModel.hxx)
ot_install_header_file (LinearModelFactory.hxx)

ot_install_header_file (OTLinearModel.hxx)
