// SWIG file ConstantNumericalMathGradientImplementation.i

%{
#include "ConstantNumericalMathGradientImplementation.hxx"
%}

%include ConstantNumericalMathGradientImplementation.hxx
namespace OT { %extend ConstantNumericalMathGradientImplementation { ConstantNumericalMathGradientImplementation(const ConstantNumericalMathGradientImplementation & other) { return new OT::ConstantNumericalMathGradientImplementation(other); } } }
