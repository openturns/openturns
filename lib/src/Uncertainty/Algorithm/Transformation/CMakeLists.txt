#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (DistributionTransformation.cxx)

ot_install_header_file (OTTransformation.hxx)
ot_install_header_file (DistributionTransformation.hxx)

# Recurse in subdirectories
add_subdirectory (MarginalTransformation)
add_subdirectory (NatafEllipticalCopula)
add_subdirectory (NatafEllipticalDistribution)
add_subdirectory (NatafIndependentCopula)
add_subdirectory (Rosenblatt)
add_subdirectory (ProcessTransformation)