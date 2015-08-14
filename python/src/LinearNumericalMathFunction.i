// SWIG file LinearNumericalMathFunction.i

%{
#include "LinearNumericalMathFunction.hxx"
%}

%include LinearNumericalMathFunction_doc.i

%include LinearNumericalMathFunction.hxx
namespace OT { %extend LinearNumericalMathFunction { LinearNumericalMathFunction(const LinearNumericalMathFunction & other) { return new OT::LinearNumericalMathFunction(other); } } }
