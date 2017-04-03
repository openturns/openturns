// SWIG file ComposedHessian.i

OTAutodoc(ComposedHessian)

%{
#include "openturns/ComposedHessian.hxx"
%}

%include openturns/ComposedHessian.hxx
namespace OT { %extend ComposedHessian { ComposedHessian(const ComposedHessian & other) { return new OT::ComposedHessian(other); } } }
