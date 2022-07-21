// SWIG file MarginalHessian.i

OTAutodoc(MarginalHessian)

%{
#include "openturns/MarginalHessian.hxx"
%}

%include openturns/MarginalHessian.hxx
namespace OT { %extend MarginalHessian { MarginalHessian(const MarginalHessian & other) { return new OT::MarginalHessian(other); } } }
