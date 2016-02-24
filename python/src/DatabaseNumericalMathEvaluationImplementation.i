// SWIG file DatabaseNumericalMathEvaluationImplementation.i

%{
#include "openturns/DatabaseNumericalMathEvaluationImplementation.hxx"
%}

%include DatabaseNumericalMathEvaluationImplementation_doc.i

%include openturns/DatabaseNumericalMathEvaluationImplementation.hxx
namespace OT { %extend DatabaseNumericalMathEvaluationImplementation { DatabaseNumericalMathEvaluationImplementation(const DatabaseNumericalMathEvaluationImplementation & other) { return new OT::DatabaseNumericalMathEvaluationImplementation(other); } } }
