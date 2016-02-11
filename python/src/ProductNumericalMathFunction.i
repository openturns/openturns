// SWIG file ProductNumericalMathFunction.i

OTAutodoc(ProductNumericalMathFunction)

%{
#include "openturns/ProductNumericalMathFunction.hxx"
%}

%include openturns/ProductNumericalMathFunction.hxx
namespace OT { %extend ProductNumericalMathFunction { ProductNumericalMathFunction(const ProductNumericalMathFunction & other) { return new OT::ProductNumericalMathFunction(other); } } }
