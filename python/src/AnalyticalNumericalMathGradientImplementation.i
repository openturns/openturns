// SWIG file AnalyticalNumericalMathGradientImplementation.i

%{
#include "AnalyticalNumericalMathGradientImplementation.hxx"
%}

%include AnalyticalNumericalMathGradientImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathGradientImplementation { AnalyticalNumericalMathGradientImplementation(const AnalyticalNumericalMathGradientImplementation & other) { return new OT::AnalyticalNumericalMathGradientImplementation(other); } } }
