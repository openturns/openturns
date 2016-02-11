// SWIG file LinearCombinationGradientImplementation.i

OTAutodoc(LinearCombinationGradientImplementation)

%{
#include "openturns/LinearCombinationGradientImplementation.hxx"
%}

%include openturns/LinearCombinationGradientImplementation.hxx
namespace OT { %extend LinearCombinationGradientImplementation { LinearCombinationGradientImplementation(const LinearCombinationGradientImplementation & other) { return new OT::LinearCombinationGradientImplementation(other); } } }
