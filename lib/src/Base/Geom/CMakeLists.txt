#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (DomainImplementation.cxx)
ot_add_source_file (Domain.cxx)
ot_add_source_file (Interval.cxx)
ot_add_source_file (IntervalMesher.cxx)
ot_add_source_file (LevelSet.cxx)
ot_add_source_file (LevelSetMesher.cxx)
ot_add_source_file (Mesh.cxx)
ot_add_source_file (MeshDomain.cxx)
ot_add_source_file (BoundaryMesher.cxx)
ot_add_source_file (RegularGrid.cxx)
ot_add_source_file (DomainComplement.cxx)
ot_add_source_file (DomainIntersection.cxx)
ot_add_source_file (DomainUnion.cxx)
ot_add_source_file (DomainDisjunctiveUnion.cxx)
ot_add_source_file (DomainDifference.cxx)

ot_install_header_file (OTGeom.hxx)
ot_install_header_file (DomainImplementation.hxx)
ot_install_header_file (Domain.hxx)
ot_install_header_file (Interval.hxx)
ot_install_header_file (IntervalMesher.hxx)
ot_install_header_file (LevelSet.hxx)
ot_install_header_file (LevelSetMesher.hxx)
ot_install_header_file (Mesh.hxx)
ot_install_header_file (MeshDomain.hxx)
ot_install_header_file (BoundaryMesher.hxx)
ot_install_header_file (RegularGrid.hxx)
ot_install_header_file (DomainComplement.hxx)
ot_install_header_file (DomainIntersection.hxx)
ot_install_header_file (DomainUnion.hxx)
ot_install_header_file (DomainDisjunctiveUnion.hxx)
ot_install_header_file (DomainDifference.hxx)
