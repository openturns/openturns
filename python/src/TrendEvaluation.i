// SWIG file TrendEvaluation.i

OTAutodoc(TrendEvaluation)

%{
#include "openturns/TrendEvaluation.hxx"
%}

%include openturns/TrendEvaluation.hxx
namespace OT { %extend TrendEvaluation { TrendEvaluation(const TrendEvaluation & other) { return new OT::TrendEvaluation(other); } } }
