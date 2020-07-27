#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (MixtureClassifier.cxx)
ot_add_source_file (MinimumVolumeClassifier.cxx)

ot_install_header_file (OTClassification.hxx)
ot_install_header_file (MixtureClassifier.hxx)
ot_install_header_file (MinimumVolumeClassifier.hxx)
