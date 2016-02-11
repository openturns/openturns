// SWIG file AnalyticalNumericalMathEvaluationImplementation.i

OTAutodoc(AnalyticalNumericalMathEvaluationImplementation)

%{
#include "openturns/AnalyticalNumericalMathEvaluationImplementation.hxx"
%}

%include openturns/AnalyticalNumericalMathEvaluationImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathEvaluationImplementation { AnalyticalNumericalMathEvaluationImplementation(const AnalyticalNumericalMathEvaluationImplementation & other) { return new OT::AnalyticalNumericalMathEvaluationImplementation(other); } } }
