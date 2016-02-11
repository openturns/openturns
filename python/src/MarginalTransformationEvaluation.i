// SWIG file MarginalTransformationEvaluation.i

%{
#include "openturns/MarginalTransformationEvaluation.hxx"
%}

%include MarginalTransformationEvaluation_doc.i

%include openturns/MarginalTransformationEvaluation.hxx
namespace OT { %extend MarginalTransformationEvaluation { MarginalTransformationEvaluation(const MarginalTransformationEvaluation & other) { return new OT::MarginalTransformationEvaluation(other); } } }
