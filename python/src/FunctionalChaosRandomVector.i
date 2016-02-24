// SWIG file FunctionalChaosRandomVector.i

%{
#include "openturns/FunctionalChaosRandomVector.hxx"
%}

%include FunctionalChaosRandomVector_doc.i

%include openturns/FunctionalChaosRandomVector.hxx
namespace OT { %extend FunctionalChaosRandomVector { FunctionalChaosRandomVector(const FunctionalChaosRandomVector & other) { return new OT::FunctionalChaosRandomVector(other); } } }
