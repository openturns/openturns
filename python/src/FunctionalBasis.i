// SWIG file FunctionalBasis.i

OTAutodoc(FunctionalBasis)

%{
#include "openturns/FunctionalBasis.hxx"
%}

%ignore OT::FunctionalBasis::operator[];

OTTypedInterfaceObjectHelper(FunctionalBasis)

%include openturns/FunctionalBasis.hxx
namespace OT { %extend FunctionalBasis { FunctionalBasis(const FunctionalBasis & other) { return new OT::FunctionalBasis(other); } } }
