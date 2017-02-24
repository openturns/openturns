// SWIG file SymbolicEvaluation.i

OTAutodoc(SymbolicEvaluation)

%{
#include "openturns/SymbolicEvaluation.hxx"
%}

%include openturns/SymbolicEvaluation.hxx
namespace OT { %extend SymbolicEvaluation { SymbolicEvaluation(const SymbolicEvaluation & other) { return new OT::SymbolicEvaluation(other); } } }
