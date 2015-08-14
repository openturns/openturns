// SWIG file NumericalMathGradientImplementation.i

%{
#include "NumericalMathGradientImplementation.hxx"
%}

%include NumericalMathGradientImplementation_doc.i

%include NumericalMathGradientImplementation.hxx
namespace OT { %extend NumericalMathGradientImplementation { NumericalMathGradientImplementation(const NumericalMathGradientImplementation & other) { return new OT::NumericalMathGradientImplementation(other); } } }
