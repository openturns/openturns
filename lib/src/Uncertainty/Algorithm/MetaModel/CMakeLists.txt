#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (MetaModelResult.cxx)
ot_add_source_file (MetaModelAlgorithm.cxx)
ot_add_source_file (MetaModelValidation.cxx)

ot_install_header_file (MetaModelResult.hxx)
ot_install_header_file (MetaModelAlgorithm.hxx)
ot_install_header_file (MetaModelValidation.hxx)

ot_install_header_file (OTMetaModel.hxx)

# Recurse in subdirectories
add_subdirectory (FunctionalChaos)
add_subdirectory (Kriging)
add_subdirectory (TensorApproximation)
add_subdirectory (LinearModel)

