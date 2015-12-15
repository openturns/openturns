// SWIG file QuadraticNumericalMathEvaluationImplementation.i

OTAutodoc(QuadraticNumericalMathEvaluationImplementation)

%{
#include "QuadraticNumericalMathEvaluationImplementation.hxx"
%}

%include QuadraticNumericalMathEvaluationImplementation.hxx
namespace OT { %extend QuadraticNumericalMathEvaluationImplementation { QuadraticNumericalMathEvaluationImplementation(const QuadraticNumericalMathEvaluationImplementation & other) { return new OT::QuadraticNumericalMathEvaluationImplementation(other); } } }
