#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (IterativeAlgorithmImplementation.cxx)
ot_add_source_file (IterativeAlgorithm.cxx)
ot_add_source_file (IterativeExtrema.cxx)
ot_add_source_file (IterativeThresholdExceedance.cxx)
ot_add_source_file (IterativeMoments.cxx)

ot_install_header_file (OTIterativeStat.hxx)
ot_install_header_file (IterativeAlgorithmImplementation.hxx)
ot_install_header_file (IterativeAlgorithm.hxx)
ot_install_header_file (IterativeExtrema.hxx)
ot_install_header_file (IterativeThresholdExceedance.hxx)
ot_install_header_file (IterativeMoments.hxx)


