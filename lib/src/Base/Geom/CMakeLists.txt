#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (DomainImplementation.cxx)
ot_add_source_file (Domain.cxx)
ot_add_source_file (Interval.cxx)
ot_add_source_file (IntervalMesher.cxx)
ot_add_source_file (LevelSet.cxx)
ot_add_source_file (Mesh.cxx)
ot_add_source_file (MeshFactory.cxx)
ot_add_source_file (MeshFactoryImplementation.cxx)
ot_add_source_file (RegularGrid.cxx)

ot_install_header_file (OTGeom.hxx)
ot_install_header_file (DomainImplementation.hxx)
ot_install_header_file (Domain.hxx)
ot_install_header_file (Interval.hxx)
ot_install_header_file (IntervalMesher.hxx)
ot_install_header_file (LevelSet.hxx)
ot_install_header_file (Mesh.hxx)
ot_install_header_file (MeshFactory.hxx)
ot_install_header_file (MeshFactoryImplementation.hxx)
ot_install_header_file (RegularGrid.hxx)
