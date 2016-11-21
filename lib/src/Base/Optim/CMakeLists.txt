#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (MultiStart.cxx)
ot_add_source_file (NearestPointChecker.cxx)
ot_add_source_file (NearestPointCheckerResult.cxx)
ot_add_source_file (OptimizationProblem.cxx)
ot_add_source_file (OptimizationProblemImplementation.cxx)
ot_add_source_file (OptimizationSolver.cxx)
ot_add_source_file (OptimizationSolverImplementation.cxx)
ot_add_source_file (OptimizationResult.cxx)
ot_add_source_file (NLopt.cxx)

ot_install_header_file (OTOptim.hxx)
ot_install_header_file (MultiStart.hxx)
ot_install_header_file (NearestPointChecker.hxx)
ot_install_header_file (NearestPointCheckerResult.hxx)
ot_install_header_file (OptimizationProblem.hxx)
ot_install_header_file (OptimizationProblemImplementation.hxx)
ot_install_header_file (OptimizationSolver.hxx)
ot_install_header_file (OptimizationSolverImplementation.hxx)
ot_install_header_file (OptimizationResult.hxx)
ot_install_header_file (NLopt.hxx)

# Recurse in subdirectories
add_subdirectory (Cobyla)
add_subdirectory (AbdoRackwitz)
add_subdirectory (TNC)
add_subdirectory (SQP)
