// SWIG file LinearNumericalMathGradientImplementation.i

%{
#include "LinearNumericalMathGradientImplementation.hxx"
%}

%include LinearNumericalMathGradientImplementation_doc.i

%include LinearNumericalMathGradientImplementation.hxx
namespace OT { %extend LinearNumericalMathGradientImplementation { LinearNumericalMathGradientImplementation(const LinearNumericalMathGradientImplementation & other) { return new OT::LinearNumericalMathGradientImplementation(other); } } }
