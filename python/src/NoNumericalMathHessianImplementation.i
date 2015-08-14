// SWIG file NoNumericalMathHessianImplementation.i

%{
#include "NoNumericalMathHessianImplementation.hxx"
%}

%include NoNumericalMathHessianImplementation.hxx
namespace OT { %extend NoNumericalMathHessianImplementation { NoNumericalMathHessianImplementation(const NoNumericalMathHessianImplementation & other) { return new OT::NoNumericalMathHessianImplementation(other); } } }
