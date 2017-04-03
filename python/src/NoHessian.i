// SWIG file NoHessian.i

OTAutodoc(NoHessian)

%{
#include "openturns/NoHessian.hxx"
%}

%include openturns/NoHessian.hxx
namespace OT { %extend NoHessian { NoHessian(const NoHessian & other) { return new OT::NoHessian(other); } } }
