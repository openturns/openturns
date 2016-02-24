// SWIG file ConstantNumericalMathGradientImplementation.i

OTAutodoc(ConstantNumericalMathGradientImplementation)

%{
#include "openturns/ConstantNumericalMathGradientImplementation.hxx"
%}

%include openturns/ConstantNumericalMathGradientImplementation.hxx
namespace OT { %extend ConstantNumericalMathGradientImplementation { ConstantNumericalMathGradientImplementation(const ConstantNumericalMathGradientImplementation & other) { return new OT::ConstantNumericalMathGradientImplementation(other); } } }
