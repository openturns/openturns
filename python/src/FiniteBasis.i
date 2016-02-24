// SWIG file FiniteBasis.i

%{
#include "openturns/FiniteBasis.hxx"
%}

%ignore OT::FiniteBasis::operator[];

%include openturns/FiniteBasis.hxx
namespace OT { %extend FiniteBasis { FiniteBasis(const FiniteBasis & other) { return new OT::FiniteBasis(other); } } }
