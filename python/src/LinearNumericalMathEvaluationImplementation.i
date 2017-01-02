// SWIG file LinearNumericalMathEvaluationImplementation.i

%{
#include "openturns/LinearNumericalMathEvaluationImplementation.hxx"
%}

%include LinearNumericalMathEvaluationImplementation_doc.i

%include openturns/LinearNumericalMathEvaluationImplementation.hxx
namespace OT { %extend LinearNumericalMathEvaluationImplementation { LinearNumericalMathEvaluationImplementation(const LinearNumericalMathEvaluationImplementation & other) { return new OT::LinearNumericalMathEvaluationImplementation(other); } } }
