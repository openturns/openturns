// SWIG file ComposedNumericalMathEvaluationImplementation.i

OTAutodoc(ComposedNumericalMathEvaluationImplementation)

%{
#include "openturns/ComposedNumericalMathEvaluationImplementation.hxx"
%}

%include openturns/ComposedNumericalMathEvaluationImplementation.hxx
namespace OT { %extend ComposedNumericalMathEvaluationImplementation { ComposedNumericalMathEvaluationImplementation(const ComposedNumericalMathEvaluationImplementation & other) { return new OT::ComposedNumericalMathEvaluationImplementation(other); } } }
