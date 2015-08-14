// SWIG file FunctionalChaosRandomVector.i

%{
#include "FunctionalChaosRandomVector.hxx"
%}

%include FunctionalChaosRandomVector_doc.i

%include FunctionalChaosRandomVector.hxx
namespace OT { %extend FunctionalChaosRandomVector { FunctionalChaosRandomVector(const FunctionalChaosRandomVector & other) { return new OT::FunctionalChaosRandomVector(other); } } }
