#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (MarginalTransformationEvaluation.cxx)
ot_add_source_file (MarginalTransformationGradient.cxx)
ot_add_source_file (MarginalTransformationHessian.cxx)

ot_install_header_file (MarginalTransformationGradient.hxx)
ot_install_header_file (MarginalTransformationEvaluation.hxx)
ot_install_header_file (MarginalTransformationHessian.hxx)
ot_install_header_file (OTMarginalTransformation.hxx)
