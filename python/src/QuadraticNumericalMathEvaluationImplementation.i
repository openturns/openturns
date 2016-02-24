// SWIG file QuadraticNumericalMathEvaluationImplementation.i

OTAutodoc(QuadraticNumericalMathEvaluationImplementation)

%{
#include "openturns/QuadraticNumericalMathEvaluationImplementation.hxx"
%}

%include openturns/QuadraticNumericalMathEvaluationImplementation.hxx
namespace OT { %extend QuadraticNumericalMathEvaluationImplementation { QuadraticNumericalMathEvaluationImplementation(const QuadraticNumericalMathEvaluationImplementation & other) { return new OT::QuadraticNumericalMathEvaluationImplementation(other); } } }
