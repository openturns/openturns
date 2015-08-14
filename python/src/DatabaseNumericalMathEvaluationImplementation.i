// SWIG file DatabaseNumericalMathEvaluationImplementation.i

%{
#include "DatabaseNumericalMathEvaluationImplementation.hxx"
%}

%include DatabaseNumericalMathEvaluationImplementation_doc.i

%include DatabaseNumericalMathEvaluationImplementation.hxx
namespace OT { %extend DatabaseNumericalMathEvaluationImplementation { DatabaseNumericalMathEvaluationImplementation(const DatabaseNumericalMathEvaluationImplementation & other) { return new OT::DatabaseNumericalMathEvaluationImplementation(other); } } }
