// SWIG file FunctionalBasisImplementation.i

%{
#include "openturns/FunctionalBasisImplementation.hxx"
%}

%ignore OT::FunctionalBasisImplementation::operator[];

%include openturns/FunctionalBasisImplementation.hxx
namespace OT { %extend FunctionalBasisImplementation { FunctionalBasisImplementation(const FunctionalBasisImplementation & other) { return new OT::FunctionalBasisImplementation(other); } } }
