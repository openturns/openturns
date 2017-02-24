// SWIG file SymbolicHessian.i

OTAutodoc(SymbolicHessian)

%{
#include "openturns/SymbolicHessian.hxx"
%}

%include openturns/SymbolicHessian.hxx
namespace OT { %extend SymbolicHessian { SymbolicHessian(const SymbolicHessian & other) { return new OT::SymbolicHessian(other); } } }
