// SWIG file ComposedNumericalMathHessianImplementation.i

OTAutodoc(ComposedNumericalMathHessianImplementation)

%{
#include "ComposedNumericalMathHessianImplementation.hxx"
%}

%include ComposedNumericalMathHessianImplementation.hxx
namespace OT { %extend ComposedNumericalMathHessianImplementation { ComposedNumericalMathHessianImplementation(const ComposedNumericalMathHessianImplementation & other) { return new OT::ComposedNumericalMathHessianImplementation(other); } } }
