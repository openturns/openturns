// SWIG file NoNumericalMathGradientImplementation.i

OTAutodoc(NoNumericalMathGradientImplementation)

%{
#include "NoNumericalMathGradientImplementation.hxx"
%}

%include NoNumericalMathGradientImplementation.hxx
namespace OT { %extend NoNumericalMathGradientImplementation { NoNumericalMathGradientImplementation(const NoNumericalMathGradientImplementation & other) { return new OT::NoNumericalMathGradientImplementation(other); } } }
