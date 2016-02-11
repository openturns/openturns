// SWIG file ProductNumericalMathHessianImplementation.i

OTAutodoc(ProductNumericalMathHessianImplementation)

%{
#include "openturns/ProductNumericalMathHessianImplementation.hxx"
%}

%include openturns/ProductNumericalMathHessianImplementation.hxx
namespace OT { %extend ProductNumericalMathHessianImplementation { ProductNumericalMathHessianImplementation(const ProductNumericalMathHessianImplementation & other) { return new OT::ProductNumericalMathHessianImplementation(other); } } }
