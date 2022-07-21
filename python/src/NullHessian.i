// SWIG file NullHessian.i

OTAutodoc(NullHessian)

%{
#include "openturns/NullHessian.hxx"
%}

%include openturns/NullHessian.hxx
namespace OT { %extend NullHessian { NullHessian(const NullHessian & other) { return new OT::NullHessian(other); } } }
