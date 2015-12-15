// SWIG file ProductNumericalMathGradientImplementation.i

OTAutodoc(ProductNumericalMathGradientImplementation)

%{
#include "ProductNumericalMathGradientImplementation.hxx"
%}

%include ProductNumericalMathGradientImplementation.hxx
namespace OT { %extend ProductNumericalMathGradientImplementation { ProductNumericalMathGradientImplementation(const ProductNumericalMathGradientImplementation & other) { return new OT::ProductNumericalMathGradientImplementation(other); } } }
