#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (CalibrationStrategyImplementation.cxx)
ot_add_source_file (CalibrationStrategy.cxx)
ot_add_source_file (SamplerImplementation.cxx)
ot_add_source_file (Sampler.cxx)
ot_add_source_file (MCMC.cxx)
ot_add_source_file (RandomWalkMetropolisHastings.cxx)
ot_add_source_file (PosteriorRandomVector.cxx)

ot_install_header_file (OTBayesian.hxx)
ot_install_header_file (CalibrationStrategyImplementation.hxx)
ot_install_header_file (CalibrationStrategy.hxx)
ot_install_header_file (SamplerImplementation.hxx)
ot_install_header_file (Sampler.hxx)
ot_install_header_file (MCMC.hxx)
ot_install_header_file (RandomWalkMetropolisHastings.hxx)
ot_install_header_file (PosteriorRandomVector.hxx)
