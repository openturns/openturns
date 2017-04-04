// SWIG file LinearCombinationHessian.i

OTAutodoc(LinearCombinationHessian)

%{
#include "openturns/LinearCombinationHessian.hxx"
%}

%include openturns/LinearCombinationHessian.hxx
namespace OT { %extend LinearCombinationHessian { LinearCombinationHessian(const LinearCombinationHessian & other) { return new OT::LinearCombinationHessian(other); } } }
