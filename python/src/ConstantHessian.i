// SWIG file ConstantHessian.i

OTAutodoc(ConstantHessian)

%{
#include "openturns/ConstantHessian.hxx"
%}

%include openturns/ConstantHessian.hxx
namespace OT { %extend ConstantHessian { ConstantHessian(const ConstantHessian & other) { return new OT::ConstantHessian(other); } } }
