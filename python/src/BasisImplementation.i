// SWIG file BasisImplementation.i

%{
#include "openturns/BasisImplementation.hxx"
%}

%include BasisImplementation_doc.i

%ignore OT::BasisImplementation::operator[];

%include openturns/BasisImplementation.hxx
namespace OT { %extend BasisImplementation { BasisImplementation(const BasisImplementation & other) { return new OT::BasisImplementation(other); } } }
