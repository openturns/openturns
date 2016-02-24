// SWIG file MarginalTransformationGradient.i

OTAutodoc(MarginalTransformationGradient)

%{
#include "openturns/MarginalTransformationGradient.hxx"
%}

%include openturns/MarginalTransformationGradient.hxx
namespace OT { %extend MarginalTransformationGradient { MarginalTransformationGradient(const MarginalTransformationGradient & other) { return new OT::MarginalTransformationGradient(other); } } }
