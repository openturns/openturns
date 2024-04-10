#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (EfficientGlobalOptimization.cxx)
ot_add_source_file (SequentialSamplingAlgorithm.cxx)
ot_add_source_file (SequentialSamplingAlgorithmImplementation.cxx)
ot_add_source_file (LOLAVoronoi.cxx)

ot_install_header_file (EfficientGlobalOptimization.hxx)
ot_install_header_file (SequentialSamplingAlgorithm.hxx)
ot_install_header_file (SequentialSamplingAlgorithmImplementation.hxx)
ot_install_header_file (LOLAVoronoi.hxx)

ot_install_header_file (OTUncertaintyOptimization.hxx)

