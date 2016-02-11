// SWIG file ProductNumericalMathGradientImplementation.i

OTAutodoc(ProductNumericalMathGradientImplementation)

%{
#include "openturns/ProductNumericalMathGradientImplementation.hxx"
%}

%include openturns/ProductNumericalMathGradientImplementation.hxx
namespace OT { %extend ProductNumericalMathGradientImplementation { ProductNumericalMathGradientImplementation(const ProductNumericalMathGradientImplementation & other) { return new OT::ProductNumericalMathGradientImplementation(other); } } }
