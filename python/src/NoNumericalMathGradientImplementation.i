// SWIG file NoNumericalMathGradientImplementation.i

OTAutodoc(NoNumericalMathGradientImplementation)

%{
#include "openturns/NoNumericalMathGradientImplementation.hxx"
%}

%include openturns/NoNumericalMathGradientImplementation.hxx
namespace OT { %extend NoNumericalMathGradientImplementation { NoNumericalMathGradientImplementation(const NoNumericalMathGradientImplementation & other) { return new OT::NoNumericalMathGradientImplementation(other); } } }
