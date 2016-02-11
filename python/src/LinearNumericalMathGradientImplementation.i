// SWIG file LinearNumericalMathGradientImplementation.i

%{
#include "openturns/LinearNumericalMathGradientImplementation.hxx"
%}

%include LinearNumericalMathGradientImplementation_doc.i

%include openturns/LinearNumericalMathGradientImplementation.hxx
namespace OT { %extend LinearNumericalMathGradientImplementation { LinearNumericalMathGradientImplementation(const LinearNumericalMathGradientImplementation & other) { return new OT::LinearNumericalMathGradientImplementation(other); } } }
