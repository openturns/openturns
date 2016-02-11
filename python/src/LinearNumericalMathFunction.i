// SWIG file LinearNumericalMathFunction.i

%{
#include "openturns/LinearNumericalMathFunction.hxx"
%}

%include LinearNumericalMathFunction_doc.i

%include openturns/LinearNumericalMathFunction.hxx
namespace OT { %extend LinearNumericalMathFunction { LinearNumericalMathFunction(const LinearNumericalMathFunction & other) { return new OT::LinearNumericalMathFunction(other); } } }
