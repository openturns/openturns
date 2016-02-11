// SWIG file NoNumericalMathEvaluationImplementation.i

OTAutodoc(NoNumericalMathEvaluationImplementation)

%{
#include "openturns/NoNumericalMathEvaluationImplementation.hxx"
%}

%include openturns/NoNumericalMathEvaluationImplementation.hxx
namespace OT { %extend NoNumericalMathEvaluationImplementation { NoNumericalMathEvaluationImplementation(const NoNumericalMathEvaluationImplementation & other) { return new OT::NoNumericalMathEvaluationImplementation(other); } } }
