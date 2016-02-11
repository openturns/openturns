// SWIG file AnalyticalNumericalMathGradientImplementation.i

OTAutodoc(AnalyticalNumericalMathGradientImplementation)

%{
#include "openturns/AnalyticalNumericalMathGradientImplementation.hxx"
%}

%include openturns/AnalyticalNumericalMathGradientImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathGradientImplementation { AnalyticalNumericalMathGradientImplementation(const AnalyticalNumericalMathGradientImplementation & other) { return new OT::AnalyticalNumericalMathGradientImplementation(other); } } }
