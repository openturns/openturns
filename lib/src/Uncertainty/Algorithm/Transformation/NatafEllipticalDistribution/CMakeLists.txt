#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (NatafEllipticalDistributionEvaluation.cxx)
ot_add_source_file (NatafEllipticalDistributionGradient.cxx)
ot_add_source_file (NatafEllipticalDistributionHessian.cxx)
ot_add_source_file (InverseNatafEllipticalDistributionEvaluation.cxx)
ot_add_source_file (InverseNatafEllipticalDistributionGradient.cxx)
ot_add_source_file (InverseNatafEllipticalDistributionHessian.cxx)

ot_install_header_file (InverseNatafEllipticalDistributionHessian.hxx)
ot_install_header_file (OTNatafEllipticalDistribution.hxx)
ot_install_header_file (InverseNatafEllipticalDistributionGradient.hxx)
ot_install_header_file (InverseNatafEllipticalDistributionEvaluation.hxx)
ot_install_header_file (NatafEllipticalDistributionHessian.hxx)
ot_install_header_file (NatafEllipticalDistributionEvaluation.hxx)
ot_install_header_file (NatafEllipticalDistributionGradient.hxx)
