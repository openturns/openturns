// SWIG file InverseTrendEvaluation.i

OTAutodoc(InverseTrendEvaluation)

%{
#include "openturns/InverseTrendEvaluation.hxx"
%}

%include openturns/InverseTrendEvaluation.hxx
namespace OT { %extend InverseTrendEvaluation { InverseTrendEvaluation(const InverseTrendEvaluation & other) { return new OT::InverseTrendEvaluation(other); } } }
