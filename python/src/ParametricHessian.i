// SWIG file ParametricHessian.i

OTAutodoc(ParametricHessian)

%{
#include "openturns/ParametricHessian.hxx"
%}

%include openturns/ParametricHessian.hxx
namespace OT { %extend ParametricHessian { ParametricHessian(const ParametricHessian & other) { return new OT::ParametricHessian(other); } } }
