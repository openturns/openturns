// SWIG file LinearCombinationGradientImplementation.i

%{
#include "LinearCombinationGradientImplementation.hxx"
%}

%include LinearCombinationGradientImplementation.hxx
namespace OT { %extend LinearCombinationGradientImplementation { LinearCombinationGradientImplementation(const LinearCombinationGradientImplementation & other) { return new OT::LinearCombinationGradientImplementation(other); } } }
