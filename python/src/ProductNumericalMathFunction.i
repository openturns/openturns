// SWIG file ProductNumericalMathFunction.i

%{
#include "ProductNumericalMathFunction.hxx"
%}

%include ProductNumericalMathFunction.hxx
namespace OT { %extend ProductNumericalMathFunction { ProductNumericalMathFunction(const ProductNumericalMathFunction & other) { return new OT::ProductNumericalMathFunction(other); } } }
