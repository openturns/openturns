// SWIG file FunctionalBasis.i

OTAutodoc(FunctionalBasis)

%{
#include "FunctionalBasis.hxx"
%}

%ignore OT::FunctionalBasis::operator[];

OTTypedInterfaceObjectHelper(FunctionalBasis)

%include FunctionalBasis.hxx
namespace OT { %extend FunctionalBasis { FunctionalBasis(const FunctionalBasis & other) { return new OT::FunctionalBasis(other); } } }
