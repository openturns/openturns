// SWIG file ComposedNumericalMathGradientImplementation.i

%{
#include "ComposedNumericalMathGradientImplementation.hxx"
%}

%include ComposedNumericalMathGradientImplementation.hxx
namespace OT { %extend ComposedNumericalMathGradientImplementation { ComposedNumericalMathGradientImplementation(const ComposedNumericalMathGradientImplementation & other) { return new OT::ComposedNumericalMathGradientImplementation(other); } } }
