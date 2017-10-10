#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (Solver.cxx)
ot_add_source_file (SolverImplementation.cxx)
ot_add_source_file (Bisection.cxx)
ot_add_source_file (Secant.cxx)
ot_add_source_file (Brent.cxx)
ot_add_source_file (ODESolver.cxx)
ot_add_source_file (ODESolverImplementation.cxx)
ot_add_source_file (RungeKutta.cxx)
ot_add_source_file (Fehlberg.cxx)

ot_install_header_file (Brent.hxx)
ot_install_header_file (Bisection.hxx)
ot_install_header_file (Secant.hxx)
ot_install_header_file (SolverImplementation.hxx)
ot_install_header_file (Solver.hxx)
ot_install_header_file (ODESolver.hxx)
ot_install_header_file (ODESolverImplementation.hxx)
ot_install_header_file (RungeKutta.hxx)
ot_install_header_file (Fehlberg.hxx)
ot_install_header_file (OTSolver.hxx)
