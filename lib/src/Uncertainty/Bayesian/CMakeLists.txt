#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (GaussianLinearCalibration.cxx)
ot_add_source_file (CalibrationAlgorithm.cxx)
ot_add_source_file (CalibrationAlgorithmImplementation.cxx)
ot_add_source_file (CalibrationResult.cxx)
ot_add_source_file (CalibrationStrategyImplementation.cxx)
ot_add_source_file (CalibrationStrategy.cxx)
ot_add_source_file (LinearLeastSquaresCalibration.cxx)
ot_add_source_file (MCMC.cxx)
ot_add_source_file (NonLinearLeastSquaresCalibration.cxx)
ot_add_source_file (RandomWalkMetropolisHastings.cxx)
ot_add_source_file (GaussianNonLinearCalibration.cxx)

ot_install_header_file (OTBayesian.hxx)
ot_install_header_file (GaussianLinearCalibration.hxx)
ot_install_header_file (CalibrationAlgorithm.hxx)
ot_install_header_file (CalibrationAlgorithmImplementation.hxx)
ot_install_header_file (CalibrationResult.hxx)
ot_install_header_file (CalibrationStrategyImplementation.hxx)
ot_install_header_file (CalibrationStrategy.hxx)
ot_install_header_file (LinearLeastSquaresCalibration.hxx)
ot_install_header_file (MCMC.hxx)
ot_install_header_file (NonLinearLeastSquaresCalibration.hxx)
ot_install_header_file (RandomWalkMetropolisHastings.hxx)
ot_install_header_file (GaussianNonLinearCalibration.hxx)
