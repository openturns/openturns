// SWIG file ComposedNumericalMathEvaluationImplementation.i

OTAutodoc(ComposedNumericalMathEvaluationImplementation)

%{
#include "ComposedNumericalMathEvaluationImplementation.hxx"
%}

%include ComposedNumericalMathEvaluationImplementation.hxx
namespace OT { %extend ComposedNumericalMathEvaluationImplementation { ComposedNumericalMathEvaluationImplementation(const ComposedNumericalMathEvaluationImplementation & other) { return new OT::ComposedNumericalMathEvaluationImplementation(other); } } }
