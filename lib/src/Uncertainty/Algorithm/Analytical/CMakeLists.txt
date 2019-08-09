#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (StrongMaximumTest.cxx)
ot_add_source_file (Analytical.cxx)
ot_add_source_file (AnalyticalResult.cxx)
ot_add_source_file (FORM.cxx)
ot_add_source_file (FORMResult.cxx)
ot_add_source_file (SORM.cxx)
ot_add_source_file (SORMResult.cxx)
ot_add_source_file (MultiFORMResult.cxx)
ot_add_source_file (SystemFORM.cxx)
ot_add_source_file (MultiFORM.cxx)

ot_install_header_file (Analytical.hxx)
ot_install_header_file (OTAnalytical.hxx)
ot_install_header_file (SORMResult.hxx)
ot_install_header_file (StrongMaximumTest.hxx)
ot_install_header_file (SORM.hxx)
ot_install_header_file (FORMResult.hxx)
ot_install_header_file (AnalyticalResult.hxx)
ot_install_header_file (FORM.hxx)
ot_install_header_file (MultiFORMResult.hxx)
ot_install_header_file (SystemFORM.hxx)
ot_install_header_file (MultiFORM.hxx)
