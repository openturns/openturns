// SWIG file NumericalMathEvaluationImplementation.i

%{
#include "NumericalMathEvaluationImplementation.hxx"
%}

%include NumericalMathEvaluationImplementation_doc.i

%include NumericalMathEvaluationImplementation.hxx
namespace OT { %extend NumericalMathEvaluationImplementation { NumericalMathEvaluationImplementation(const NumericalMathEvaluationImplementation & other) { return new OT::NumericalMathEvaluationImplementation(other); } } }
