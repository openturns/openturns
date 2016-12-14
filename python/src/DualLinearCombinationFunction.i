// SWIG file DualLinearCombinationFunction.i

%{
#include "openturns/DualLinearCombinationFunction.hxx"
%}

%include DualLinearCombinationFunction_doc.i

%include openturns/DualLinearCombinationFunction.hxx
namespace OT { %extend DualLinearCombinationFunction { DualLinearCombinationFunction(const DualLinearCombinationFunction & other) { return new OT::DualLinearCombinationFunction(other); } } }
