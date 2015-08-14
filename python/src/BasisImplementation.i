// SWIG file BasisImplementation.i

%{
#include "BasisImplementation.hxx"
%}

%include BasisImplementation_doc.i

%ignore OT::BasisImplementation::operator[];

%include BasisImplementation.hxx
namespace OT { %extend BasisImplementation { BasisImplementation(const BasisImplementation & other) { return new OT::BasisImplementation(other); } } }
