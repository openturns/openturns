// SWIG file ProductNumericalMathHessianImplementation.i

%{
#include "ProductNumericalMathHessianImplementation.hxx"
%}

%include ProductNumericalMathHessianImplementation.hxx
namespace OT { %extend ProductNumericalMathHessianImplementation { ProductNumericalMathHessianImplementation(const ProductNumericalMathHessianImplementation & other) { return new OT::ProductNumericalMathHessianImplementation(other); } } }
