// SWIG file FiniteBasis.i

%{
#include "FiniteBasis.hxx"
%}

%ignore OT::FiniteBasis::operator[];

%include FiniteBasis.hxx
namespace OT { %extend FiniteBasis { FiniteBasis(const FiniteBasis & other) { return new OT::FiniteBasis(other); } } }
