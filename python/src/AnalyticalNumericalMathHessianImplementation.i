// SWIG file AnalyticalNumericalMathHessianImplementation.i

OTAutodoc(AnalyticalNumericalMathHessianImplementation)

%{
#include "AnalyticalNumericalMathHessianImplementation.hxx"
%}

%include AnalyticalNumericalMathHessianImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathHessianImplementation { AnalyticalNumericalMathHessianImplementation(const AnalyticalNumericalMathHessianImplementation & other) { return new OT::AnalyticalNumericalMathHessianImplementation(other); } } }
