#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (NearestPointAlgorithm.cxx)
ot_add_source_file (NearestPointAlgorithmImplementation.cxx)
ot_add_source_file (NearestPointAlgorithmImplementationResult.cxx)
ot_add_source_file (NearestPointChecker.cxx)
ot_add_source_file (NearestPointCheckerResult.cxx)
ot_add_source_file (BoundConstrainedAlgorithm.cxx)
ot_add_source_file (BoundConstrainedAlgorithmImplementation.cxx)
ot_add_source_file (BoundConstrainedAlgorithmImplementationResult.cxx)
ot_add_source_file (OptimizationProblem.cxx)
ot_add_source_file (OptimizationProblemImplementation.cxx)

ot_install_header_file (BoundConstrainedAlgorithm.hxx)
ot_install_header_file (NearestPointAlgorithm.hxx)
ot_install_header_file (NearestPointAlgorithmImplementation.hxx)
ot_install_header_file (NearestPointAlgorithmImplementationResult.hxx)
ot_install_header_file (OTOptim.hxx)
ot_install_header_file (BoundConstrainedAlgorithmImplementationResult.hxx)
ot_install_header_file (BoundConstrainedAlgorithmImplementation.hxx)
ot_install_header_file (NearestPointChecker.hxx)
ot_install_header_file (NearestPointCheckerResult.hxx)
ot_install_header_file (OptimizationProblem.hxx)
ot_install_header_file (OptimizationProblemImplementation.hxx)

# Recurse in subdirectories
add_subdirectory (Cobyla)
add_subdirectory (AbdoRackwitz)
add_subdirectory (TNC)
add_subdirectory (SQP)
