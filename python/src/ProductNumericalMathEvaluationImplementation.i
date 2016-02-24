// SWIG file ProductNumericalMathEvaluationImplementation.i

OTAutodoc(ProductNumericalMathEvaluationImplementation)

%{
#include "openturns/ProductNumericalMathEvaluationImplementation.hxx"
%}

%include openturns/ProductNumericalMathEvaluationImplementation.hxx
namespace OT { %extend ProductNumericalMathEvaluationImplementation { ProductNumericalMathEvaluationImplementation(const ProductNumericalMathEvaluationImplementation & other) { return new OT::ProductNumericalMathEvaluationImplementation(other); } } }
