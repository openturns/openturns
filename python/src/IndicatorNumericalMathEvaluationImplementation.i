// SWIG file IndicatorNumericalMathEvaluationImplementation.i

OTAutodoc(IndicatorNumericalMathEvaluationImplementation)

%{
#include "openturns/IndicatorNumericalMathEvaluationImplementation.hxx"
%}

%include openturns/IndicatorNumericalMathEvaluationImplementation.hxx
namespace OT { %extend IndicatorNumericalMathEvaluationImplementation { IndicatorNumericalMathEvaluationImplementation(const IndicatorNumericalMathEvaluationImplementation & other) { return new OT::IndicatorNumericalMathEvaluationImplementation(other); } } }
