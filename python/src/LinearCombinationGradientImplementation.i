// SWIG file LinearCombinationGradientImplementation.i

OTAutodoc(LinearCombinationGradientImplementation)

%{
#include "LinearCombinationGradientImplementation.hxx"
%}

%include LinearCombinationGradientImplementation.hxx
namespace OT { %extend LinearCombinationGradientImplementation { LinearCombinationGradientImplementation(const LinearCombinationGradientImplementation & other) { return new OT::LinearCombinationGradientImplementation(other); } } }
