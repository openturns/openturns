// SWIG file QuadraticNumericalMathFunction.i

OTAutodoc(QuadraticNumericalMathFunction)

%{
#include "openturns/QuadraticNumericalMathFunction.hxx"
%}

%include openturns/QuadraticNumericalMathFunction.hxx
namespace OT { %extend QuadraticNumericalMathFunction { QuadraticNumericalMathFunction(const QuadraticNumericalMathFunction & other) { return new OT::QuadraticNumericalMathFunction(other); } } }
