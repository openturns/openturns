#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (MetaModelResult.cxx)
ot_add_source_file (MetaModelAlgorithm.cxx)
ot_add_source_file (KrigingEvaluation.cxx)
ot_add_source_file (KrigingGradient.cxx)
ot_add_source_file (KrigingAlgorithm.cxx)
ot_add_source_file (KrigingResult.cxx)
ot_add_source_file (GeneralizedLinearModelResult.cxx)
ot_add_source_file (GeneralizedLinearModelAlgorithm.cxx)
ot_add_source_file (MetaModelValidation.cxx)

ot_install_header_file (MetaModelResult.hxx)
ot_install_header_file (MetaModelAlgorithm.hxx)
ot_install_header_file (KrigingEvaluation.hxx)
ot_install_header_file (KrigingGradient.hxx)
ot_install_header_file (KrigingAlgorithm.hxx)
ot_install_header_file (KrigingResult.hxx)
ot_install_header_file (GeneralizedLinearModelResult.hxx)
ot_install_header_file (GeneralizedLinearModelAlgorithm.hxx)
ot_install_header_file (MetaModelValidation.hxx)

ot_install_header_file (OTMetaModel.hxx)

# Recurse in subdirectories
add_subdirectory (FunctionalChaos)
