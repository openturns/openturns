// SWIG file MarginalEvaluation.i

OTAutodoc(MarginalEvaluation)

%{
#include "openturns/MarginalEvaluation.hxx"
%}

%include openturns/MarginalEvaluation.hxx
namespace OT { %extend MarginalEvaluation { MarginalEvaluation(const MarginalEvaluation & other) { return new OT::MarginalEvaluation(other); } } }
