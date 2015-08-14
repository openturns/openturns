#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_install_header_file (OTAlgorithm.hxx)

# Recurse in subdirectories
add_subdirectory (Analytical)
add_subdirectory (Classification)
add_subdirectory (MetaModel)
add_subdirectory (OrthogonalBasis)
add_subdirectory (QuadraticCumul)
add_subdirectory (Sensitivity)
add_subdirectory (Simulation)
add_subdirectory (Transformation)
add_subdirectory (WeightedExperiments)

