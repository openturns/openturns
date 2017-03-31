#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (CanonicalTensorEvaluation.cxx)
ot_add_source_file (CanonicalTensorGradient.cxx)
ot_add_source_file (TensorApproximationResult.cxx)
ot_add_source_file (TensorApproximationAlgorithm.cxx)

ot_install_header_file (OTTensorApproximation.hxx)

ot_install_header_file (CanonicalTensorEvaluation.hxx)
ot_install_header_file (CanonicalTensorGradient.hxx)
ot_install_header_file (TensorApproximationResult.hxx)
ot_install_header_file (TensorApproximationAlgorithm.hxx)

