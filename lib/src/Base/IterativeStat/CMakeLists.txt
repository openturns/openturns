#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (IterativeAlgorithmImplementation.cxx)
ot_add_source_file (IterativeAlgorithm.cxx)
ot_add_source_file (IterativeQuantile.cxx)
ot_add_source_file (IterativeExtrema.cxx)
ot_add_source_file (IterativeThresholdExceedance.cxx)
ot_add_source_file (IterativeKurtosis.cxx)
ot_add_source_file (IterativeSkewness.cxx)
ot_add_source_file (IterativeVariance.cxx)
ot_add_source_file (IterativeMean.cxx)

ot_install_header_file (OTIterativeStat.hxx)
ot_install_header_file (IterativeAlgorithmImplementation.hxx)
ot_install_header_file (IterativeAlgorithm.hxx)
ot_install_header_file (IterativeQuantile.hxx)
ot_install_header_file (IterativeExtrema.hxx)
ot_install_header_file (IterativeThresholdExceedance.hxx)
ot_install_header_file (IterativeKurtosis.hxx)
ot_install_header_file (IterativeSkewness.hxx)
ot_install_header_file (IterativeVariance.hxx)
ot_install_header_file (IterativeMean.hxx)


