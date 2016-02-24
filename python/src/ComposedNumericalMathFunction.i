// SWIG file ComposedNumericalMathFunction.i

OTAutodoc(ComposedNumericalMathFunction)

%{
#include "openturns/ComposedNumericalMathFunction.hxx"
%}

%include openturns/ComposedNumericalMathFunction.hxx
namespace OT { %extend ComposedNumericalMathFunction { ComposedNumericalMathFunction(const ComposedNumericalMathFunction & other) { return new OT::ComposedNumericalMathFunction(other); } } }
