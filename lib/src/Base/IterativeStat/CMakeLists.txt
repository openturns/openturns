#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (IterativeAlgorithm.cxx)
ot_add_source_file (MelissaStudy.cxx)
ot_add_source_file (IterativeMeanImplementation.cxx)
ot_add_source_file (IterativeMean.cxx)

ot_install_header_file (OTIterativeStat.hxx)
ot_install_header_file (IterativeAlgorithm.hxx)
ot_install_header_file (MelissaStudy.hxx)
ot_install_header_file (IterativeMeanImplementation.hxx)
ot_install_header_file (IterativeMean.hxx)


