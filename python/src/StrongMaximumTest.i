// SWIG file StrongMaximumTest.i

%{
#include "openturns/StrongMaximumTest.hxx"
%}

%include StrongMaximumTest_doc.i

%include openturns/StrongMaximumTest.hxx
namespace OT{ %extend StrongMaximumTest { StrongMaximumTest(const StrongMaximumTest & other) { return new OT::StrongMaximumTest(other); } } }
