// SWIG file IndicatorNumericalMathEvaluationImplementation.i

%{
#include "IndicatorNumericalMathEvaluationImplementation.hxx"
%}

%include IndicatorNumericalMathEvaluationImplementation.hxx
namespace OT { %extend IndicatorNumericalMathEvaluationImplementation { IndicatorNumericalMathEvaluationImplementation(const IndicatorNumericalMathEvaluationImplementation & other) { return new OT::IndicatorNumericalMathEvaluationImplementation(other); } } }
