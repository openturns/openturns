// SWIG file ComposedNumericalMathEvaluationImplementation.i

%{
#include "ComposedNumericalMathEvaluationImplementation.hxx"
%}

%include ComposedNumericalMathEvaluationImplementation.hxx
namespace OT { %extend ComposedNumericalMathEvaluationImplementation { ComposedNumericalMathEvaluationImplementation(const ComposedNumericalMathEvaluationImplementation & other) { return new OT::ComposedNumericalMathEvaluationImplementation(other); } } }
