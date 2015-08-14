#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (NatafIndependentCopulaEvaluation.cxx)
ot_add_source_file (NatafIndependentCopulaGradient.cxx)
ot_add_source_file (NatafIndependentCopulaHessian.cxx)
ot_add_source_file (InverseNatafIndependentCopulaEvaluation.cxx)
ot_add_source_file (InverseNatafIndependentCopulaGradient.cxx)
ot_add_source_file (InverseNatafIndependentCopulaHessian.cxx)

ot_install_header_file (NatafIndependentCopulaGradient.hxx)
ot_install_header_file (InverseNatafIndependentCopulaEvaluation.hxx)
ot_install_header_file (InverseNatafIndependentCopulaHessian.hxx)
ot_install_header_file (OTNatafIndependentCopula.hxx)
ot_install_header_file (NatafIndependentCopulaHessian.hxx)
ot_install_header_file (NatafIndependentCopulaEvaluation.hxx)
ot_install_header_file (InverseNatafIndependentCopulaGradient.hxx)
