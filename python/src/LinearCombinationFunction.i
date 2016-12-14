// SWIG file LinearCombinationFunction.i

%{
#include "openturns/LinearCombinationFunction.hxx"
%}

%include LinearCombinationFunction_doc.i

%include openturns/LinearCombinationFunction.hxx
namespace OT { %extend LinearCombinationFunction { LinearCombinationFunction(const LinearCombinationFunction & other) { return new OT::LinearCombinationFunction(other); } } }
