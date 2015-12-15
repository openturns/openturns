// SWIG file ProductNumericalMathEvaluationImplementation.i

OTAutodoc(ProductNumericalMathEvaluationImplementation)

%{
#include "ProductNumericalMathEvaluationImplementation.hxx"
%}

%include ProductNumericalMathEvaluationImplementation.hxx
namespace OT { %extend ProductNumericalMathEvaluationImplementation { ProductNumericalMathEvaluationImplementation(const ProductNumericalMathEvaluationImplementation & other) { return new OT::ProductNumericalMathEvaluationImplementation(other); } } }
