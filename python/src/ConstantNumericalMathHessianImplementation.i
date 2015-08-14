// SWIG file ConstantNumericalMathHessianImplementation.i

%{
#include "ConstantNumericalMathHessianImplementation.hxx"
%}

%include ConstantNumericalMathHessianImplementation.hxx
namespace OT { %extend ConstantNumericalMathHessianImplementation { ConstantNumericalMathHessianImplementation(const ConstantNumericalMathHessianImplementation & other) { return new OT::ConstantNumericalMathHessianImplementation(other); } } }
