// SWIG file FunctionalBasisImplementation.i

%{
#include "FunctionalBasisImplementation.hxx"
%}

%ignore OT::FunctionalBasisImplementation::operator[];

%include FunctionalBasisImplementation.hxx
namespace OT { %extend FunctionalBasisImplementation { FunctionalBasisImplementation(const FunctionalBasisImplementation & other) { return new OT::FunctionalBasisImplementation(other); } } }
