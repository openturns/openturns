// SWIG file StrongMaximumTest.i

%{
#include "StrongMaximumTest.hxx"
%}

%include StrongMaximumTest_doc.i

%include StrongMaximumTest.hxx
namespace OT{ %extend StrongMaximumTest { StrongMaximumTest(const StrongMaximumTest & other) { return new OT::StrongMaximumTest(other); } } }
