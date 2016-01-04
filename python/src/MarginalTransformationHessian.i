// SWIG file MarginalTransformationHessian.i

OTAutodoc(MarginalTransformationHessian)

%{
#include "MarginalTransformationHessian.hxx"
%}

%include MarginalTransformationHessian.hxx
namespace OT { %extend MarginalTransformationHessian { MarginalTransformationHessian(const MarginalTransformationHessian & other) { return new OT::MarginalTransformationHessian(other); } } }
