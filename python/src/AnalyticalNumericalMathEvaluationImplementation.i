// SWIG file AnalyticalNumericalMathEvaluationImplementation.i

%{
#include "AnalyticalNumericalMathEvaluationImplementation.hxx"
%}

%include AnalyticalNumericalMathEvaluationImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathEvaluationImplementation { AnalyticalNumericalMathEvaluationImplementation(const AnalyticalNumericalMathEvaluationImplementation & other) { return new OT::AnalyticalNumericalMathEvaluationImplementation(other); } } }
