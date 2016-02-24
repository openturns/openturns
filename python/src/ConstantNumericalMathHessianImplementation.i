// SWIG file ConstantNumericalMathHessianImplementation.i

OTAutodoc(ConstantNumericalMathHessianImplementation)

%{
#include "openturns/ConstantNumericalMathHessianImplementation.hxx"
%}

%include openturns/ConstantNumericalMathHessianImplementation.hxx
namespace OT { %extend ConstantNumericalMathHessianImplementation { ConstantNumericalMathHessianImplementation(const ConstantNumericalMathHessianImplementation & other) { return new OT::ConstantNumericalMathHessianImplementation(other); } } }
