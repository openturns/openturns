// SWIG file ComposedNumericalMathGradientImplementation.i

OTAutodoc(ComposedNumericalMathGradientImplementation)

%{
#include "openturns/ComposedNumericalMathGradientImplementation.hxx"
%}

%include openturns/ComposedNumericalMathGradientImplementation.hxx
namespace OT { %extend ComposedNumericalMathGradientImplementation { ComposedNumericalMathGradientImplementation(const ComposedNumericalMathGradientImplementation & other) { return new OT::ComposedNumericalMathGradientImplementation(other); } } }
