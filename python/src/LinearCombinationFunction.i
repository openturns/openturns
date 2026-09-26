// SWIG file LinearCombinationFunction.i

%{
#include "openturns/LinearCombinationFunction.hxx"
%}

%include LinearCombinationFunction_doc.i

%copyctor OT::LinearCombinationFunction;

%ignore OT::LinearCombinationFunction::mergeLinearCombination;
%ignore OT::LinearCombinationFunction::add;
%ignore OT::LinearCombinationFunction::negate;

%include openturns/LinearCombinationFunction.hxx
