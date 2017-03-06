// SWIG file SymbolicGradient.i

OTAutodoc(SymbolicGradient)

%{
#include "openturns/SymbolicGradient.hxx"
%}

%include openturns/SymbolicGradient.hxx
namespace OT { %extend SymbolicGradient { SymbolicGradient(const SymbolicGradient & other) { return new OT::SymbolicGradient(other); } } }
