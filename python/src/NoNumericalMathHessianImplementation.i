// SWIG file NoNumericalMathHessianImplementation.i

OTAutodoc(NoNumericalMathHessianImplementation)

%{
#include "openturns/NoNumericalMathHessianImplementation.hxx"
%}

%include openturns/NoNumericalMathHessianImplementation.hxx
namespace OT { %extend NoNumericalMathHessianImplementation { NoNumericalMathHessianImplementation(const NoNumericalMathHessianImplementation & other) { return new OT::NoNumericalMathHessianImplementation(other); } } }
