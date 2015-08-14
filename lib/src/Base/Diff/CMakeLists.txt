#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (FiniteDifferenceStepImplementation.cxx)
ot_add_source_file (FiniteDifferenceStep.cxx)
ot_add_source_file (ConstantStep.cxx)
ot_add_source_file (BlendedStep.cxx)
ot_add_source_file (FiniteDifferenceGradient.cxx)
ot_add_source_file (FiniteDifferenceHessian.cxx)
ot_add_source_file (NonCenteredFiniteDifferenceGradient.cxx)
ot_add_source_file (CenteredFiniteDifferenceGradient.cxx)
ot_add_source_file (CenteredFiniteDifferenceHessian.cxx)

ot_install_header_file (FiniteDifferenceStepImplementation.hxx)
ot_install_header_file (FiniteDifferenceStep.hxx)
ot_install_header_file (ConstantStep.hxx)
ot_install_header_file (BlendedStep.hxx)
ot_install_header_file (FiniteDifferenceGradient.hxx)
ot_install_header_file (FiniteDifferenceHessian.hxx)
ot_install_header_file (NonCenteredFiniteDifferenceGradient.hxx)
ot_install_header_file (CenteredFiniteDifferenceGradient.hxx)
ot_install_header_file (OTDiff.hxx)
ot_install_header_file (CenteredFiniteDifferenceHessian.hxx)

# Recurse in subdirectories
add_subdirectory (Ev3)