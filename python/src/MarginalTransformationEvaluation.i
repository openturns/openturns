// SWIG file MarginalTransformationEvaluation.i

%{
#include "MarginalTransformationEvaluation.hxx"
%}

%include MarginalTransformationEvaluation_doc.i

%include MarginalTransformationEvaluation.hxx
namespace OT { %extend MarginalTransformationEvaluation { MarginalTransformationEvaluation(const MarginalTransformationEvaluation & other) { return new OT::MarginalTransformationEvaluation(other); } } }
