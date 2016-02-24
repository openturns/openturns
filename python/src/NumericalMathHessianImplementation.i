// SWIG file NumericalMathHessianImplementation.i

%{
#include "openturns/NumericalMathHessianImplementation.hxx"
%}

%include NumericalMathHessianImplementation_doc.i

%include openturns/NumericalMathHessianImplementation.hxx
namespace OT { %extend NumericalMathHessianImplementation { NumericalMathHessianImplementation(const NumericalMathHessianImplementation & other) { return new OT::NumericalMathHessianImplementation(other); } } }
