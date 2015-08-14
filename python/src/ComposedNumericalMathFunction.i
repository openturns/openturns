// SWIG file ComposedNumericalMathFunction.i

%{
#include "ComposedNumericalMathFunction.hxx"
%}

%include ComposedNumericalMathFunction.hxx
namespace OT { %extend ComposedNumericalMathFunction { ComposedNumericalMathFunction(const ComposedNumericalMathFunction & other) { return new OT::ComposedNumericalMathFunction(other); } } }
