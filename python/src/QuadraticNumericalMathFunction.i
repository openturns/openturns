// SWIG file QuadraticNumericalMathFunction.i

OTAutodoc(QuadraticNumericalMathFunction)

%{
#include "QuadraticNumericalMathFunction.hxx"
%}

%include QuadraticNumericalMathFunction.hxx
namespace OT { %extend QuadraticNumericalMathFunction { QuadraticNumericalMathFunction(const QuadraticNumericalMathFunction & other) { return new OT::QuadraticNumericalMathFunction(other); } } }
