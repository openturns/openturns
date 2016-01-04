// SWIG file ConstantNumericalMathGradientImplementation.i

OTAutodoc(ConstantNumericalMathGradientImplementation)

%{
#include "ConstantNumericalMathGradientImplementation.hxx"
%}

%include ConstantNumericalMathGradientImplementation.hxx
namespace OT { %extend ConstantNumericalMathGradientImplementation { ConstantNumericalMathGradientImplementation(const ConstantNumericalMathGradientImplementation & other) { return new OT::ConstantNumericalMathGradientImplementation(other); } } }
