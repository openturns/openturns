// SWIG file MarginalTransformationEvaluation.i

%{
#include "MarginalTransformationEvaluation.hxx"
%}

%include MarginalTransformationEvaluation.hxx
namespace OT { %extend MarginalTransformationEvaluation { MarginalTransformationEvaluation(const MarginalTransformationEvaluation & other) { return new OT::MarginalTransformationEvaluation(other); } } }
