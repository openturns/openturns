#                                               -*- cmake -*-

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (FittingTest.cxx)
ot_add_source_file (VisualTest.cxx)
ot_add_source_file (NormalityTest.cxx)
ot_add_source_file (LinearModelTest.cxx)
ot_add_source_file (HypothesisTest.cxx)
ot_add_source_file (DickeyFullerTest.cxx)

ot_install_header_file (NormalityTest.hxx)
ot_install_header_file (LinearModelTest.hxx)
ot_install_header_file (VisualTest.hxx)
ot_install_header_file (FittingTest.hxx)
ot_install_header_file (HypothesisTest.hxx)
ot_install_header_file (DickeyFullerTest.hxx)
ot_install_header_file (OTStatTests.hxx)
