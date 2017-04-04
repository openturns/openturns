// SWIG file IndicatorEvaluation.i

OTAutodoc(IndicatorEvaluation)

%{
#include "openturns/IndicatorEvaluation.hxx"
%}

%include openturns/IndicatorEvaluation.hxx
namespace OT { %extend IndicatorEvaluation { IndicatorEvaluation(const IndicatorEvaluation & other) { return new OT::IndicatorEvaluation(other); } } }
