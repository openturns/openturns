// SWIG file AggregatedNumericalMathEvaluationImplementation.i

%{
#include "openturns/AggregatedNumericalMathEvaluationImplementation.hxx"
%}

%include AggregatedNumericalMathEvaluationImplementation_doc.i

%include openturns/AggregatedNumericalMathEvaluationImplementation.hxx

namespace OT { %extend AggregatedNumericalMathEvaluationImplementation { AggregatedNumericalMathEvaluationImplementation(const AggregatedNumericalMathEvaluationImplementation & other) { return new OT::AggregatedNumericalMathEvaluationImplementation(other); } } }
