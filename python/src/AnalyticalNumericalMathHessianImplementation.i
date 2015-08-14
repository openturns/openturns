// SWIG file AnalyticalNumericalMathHessianImplementation.i

%{
#include "AnalyticalNumericalMathHessianImplementation.hxx"
%}

%include AnalyticalNumericalMathHessianImplementation.hxx
namespace OT { %extend AnalyticalNumericalMathHessianImplementation { AnalyticalNumericalMathHessianImplementation(const AnalyticalNumericalMathHessianImplementation & other) { return new OT::AnalyticalNumericalMathHessianImplementation(other); } } }
