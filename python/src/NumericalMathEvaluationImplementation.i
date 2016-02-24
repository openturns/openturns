// SWIG file NumericalMathEvaluationImplementation.i

%{
#include "openturns/NumericalMathEvaluationImplementation.hxx"
%}

%include NumericalMathEvaluationImplementation_doc.i

%include openturns/NumericalMathEvaluationImplementation.hxx
namespace OT { %extend NumericalMathEvaluationImplementation { NumericalMathEvaluationImplementation(const NumericalMathEvaluationImplementation & other) { return new OT::NumericalMathEvaluationImplementation(other); } } }
