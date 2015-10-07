#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (Debye.cxx)
ot_add_source_file (BetaFunctions.cxx)
ot_add_source_file (ExponentialIntegralFunctions.cxx)
ot_add_source_file (Faddeeva.cc)
ot_add_source_file (GammaFunctions.cxx)
ot_add_source_file (SpecFunc.cxx)
ot_add_source_file (incgam.cxx)

ot_install_header_file (SpecFunc.hxx)
ot_install_header_file (BetaFunctions.hxx)
ot_install_header_file (ExponentialIntegralFunctions.hxx)
ot_install_header_file (GammaFunctions.hxx)
ot_install_header_file (OTSpecFunc.hxx)
