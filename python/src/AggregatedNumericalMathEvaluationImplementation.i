// SWIG file AggregatedNumericalMathEvaluationImplementation.i

%{
#include "AggregatedNumericalMathEvaluationImplementation.hxx"
%}

%include AggregatedNumericalMathEvaluationImplementation_doc.i

%include AggregatedNumericalMathEvaluationImplementation.hxx

namespace OT { %extend AggregatedNumericalMathEvaluationImplementation { AggregatedNumericalMathEvaluationImplementation(const AggregatedNumericalMathEvaluationImplementation & other) { return new OT::AggregatedNumericalMathEvaluationImplementation(other); } } }
