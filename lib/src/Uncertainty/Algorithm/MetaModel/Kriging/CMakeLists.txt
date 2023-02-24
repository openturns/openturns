#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()


ot_add_source_file (KrigingEvaluation.cxx)
ot_add_source_file (KrigingGradient.cxx)
ot_add_source_file (KrigingAlgorithm.cxx)
ot_add_source_file (KrigingResult.cxx)
ot_add_source_file (GeneralLinearModelResult.cxx)
ot_add_source_file (GeneralLinearModelAlgorithm.cxx)
ot_add_source_file (GaussianProcessFitterResult.cxx)
ot_add_source_file (GaussianProcessFitter.cxx)
ot_add_source_file (GaussianProcessRegressionResult.cxx)


ot_install_header_file (KrigingEvaluation.hxx)
ot_install_header_file (KrigingGradient.hxx)
ot_install_header_file (KrigingAlgorithm.hxx)
ot_install_header_file (KrigingResult.hxx)
ot_install_header_file (GeneralLinearModelResult.hxx)
ot_install_header_file (GeneralLinearModelAlgorithm.hxx)
ot_install_header_file (GaussianProcessFitterResult.hxx)
ot_install_header_file (GaussianProcessFitter.hxx)
ot_install_header_file (GaussianProcessRegressionResult.hxx)

ot_install_header_file (OTKriging.hxx)
