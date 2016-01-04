// SWIG file ComposedNumericalMathGradientImplementation.i

OTAutodoc(ComposedNumericalMathGradientImplementation)

%{
#include "ComposedNumericalMathGradientImplementation.hxx"
%}

%include ComposedNumericalMathGradientImplementation.hxx
namespace OT { %extend ComposedNumericalMathGradientImplementation { ComposedNumericalMathGradientImplementation(const ComposedNumericalMathGradientImplementation & other) { return new OT::ComposedNumericalMathGradientImplementation(other); } } }
