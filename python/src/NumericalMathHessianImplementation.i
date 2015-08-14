// SWIG file NumericalMathHessianImplementation.i

%{
#include "NumericalMathHessianImplementation.hxx"
%}

%include NumericalMathHessianImplementation_doc.i

%include NumericalMathHessianImplementation.hxx
namespace OT { %extend NumericalMathHessianImplementation { NumericalMathHessianImplementation(const NumericalMathHessianImplementation & other) { return new OT::NumericalMathHessianImplementation(other); } } }
