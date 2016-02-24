// SWIG file AnalyticalNumericalMathHessianImplementation.i

OTAutodoc(AnalyticalNumericalMathHessianImplementation)

%{
#include "openturns/AnalyticalNumericalMathHessianImplementation.hxx"
%}

%include openturns/AnalyticalNumericalMathHessianImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathHessianImplementation { AnalyticalNumericalMathHessianImplementation(const AnalyticalNumericalMathHessianImplementation & other) { return new OT::AnalyticalNumericalMathHessianImplementation(other); } } }
