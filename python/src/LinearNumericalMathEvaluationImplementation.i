// SWIG file LinearNumericalMathEvaluationImplementation.i

%{
#include "LinearNumericalMathEvaluationImplementation.hxx"
%}

%include LinearNumericalMathEvaluationImplementation_doc.i

%include LinearNumericalMathEvaluationImplementation.hxx
namespace OT { %extend LinearNumericalMathEvaluationImplementation { LinearNumericalMathEvaluationImplementation(const LinearNumericalMathEvaluationImplementation & other) { return new OT::LinearNumericalMathEvaluationImplementation(other); } } }
