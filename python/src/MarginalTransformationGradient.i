// SWIG file MarginalTransformationGradient.i

OTAutodoc(MarginalTransformationGradient)

%{
#include "MarginalTransformationGradient.hxx"
%}

%include MarginalTransformationGradient.hxx
namespace OT { %extend MarginalTransformationGradient { MarginalTransformationGradient(const MarginalTransformationGradient & other) { return new OT::MarginalTransformationGradient(other); } } }
