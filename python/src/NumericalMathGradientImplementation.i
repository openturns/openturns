// SWIG file NumericalMathGradientImplementation.i

%{
#include "openturns/NumericalMathGradientImplementation.hxx"
%}

%include NumericalMathGradientImplementation_doc.i

%include openturns/NumericalMathGradientImplementation.hxx
namespace OT { %extend NumericalMathGradientImplementation { NumericalMathGradientImplementation(const NumericalMathGradientImplementation & other) { return new OT::NumericalMathGradientImplementation(other); } } }
