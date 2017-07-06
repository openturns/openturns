#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (CalibrationStrategyImplementation.cxx)
ot_add_source_file (CalibrationStrategy.cxx)
ot_add_source_file (MCMC.cxx)
ot_add_source_file (RandomWalkMetropolisHastings.cxx)

ot_install_header_file (OTBayesian.hxx)
ot_install_header_file (CalibrationStrategyImplementation.hxx)
ot_install_header_file (CalibrationStrategy.hxx)
ot_install_header_file (MCMC.hxx)
ot_install_header_file (RandomWalkMetropolisHastings.hxx)
