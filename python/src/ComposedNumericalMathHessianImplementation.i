// SWIG file ComposedNumericalMathHessianImplementation.i

OTAutodoc(ComposedNumericalMathHessianImplementation)

%{
#include "openturns/ComposedNumericalMathHessianImplementation.hxx"
%}

%include openturns/ComposedNumericalMathHessianImplementation.hxx
namespace OT { %extend ComposedNumericalMathHessianImplementation { ComposedNumericalMathHessianImplementation(const ComposedNumericalMathHessianImplementation & other) { return new OT::ComposedNumericalMathHessianImplementation(other); } } }
