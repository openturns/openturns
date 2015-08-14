#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (NatafEllipticalCopulaEvaluation.cxx)
ot_add_source_file (NatafEllipticalCopulaGradient.cxx)
ot_add_source_file (NatafEllipticalCopulaHessian.cxx)
ot_add_source_file (InverseNatafEllipticalCopulaEvaluation.cxx)
ot_add_source_file (InverseNatafEllipticalCopulaGradient.cxx)
ot_add_source_file (InverseNatafEllipticalCopulaHessian.cxx)

ot_install_header_file (InverseNatafEllipticalCopulaGradient.hxx)
ot_install_header_file (NatafEllipticalCopulaEvaluation.hxx)
ot_install_header_file (InverseNatafEllipticalCopulaEvaluation.hxx)
ot_install_header_file (NatafEllipticalCopulaGradient.hxx)
ot_install_header_file (NatafEllipticalCopulaHessian.hxx)
ot_install_header_file (InverseNatafEllipticalCopulaHessian.hxx)
ot_install_header_file (OTNatafEllipticalCopula.hxx)
