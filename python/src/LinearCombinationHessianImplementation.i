// SWIG file LinearCombinationHessianImplementation.i

OTAutodoc(LinearCombinationHessianImplementation)

%{
#include "openturns/LinearCombinationHessianImplementation.hxx"
%}

%include openturns/LinearCombinationHessianImplementation.hxx
namespace OT { %extend LinearCombinationHessianImplementation { LinearCombinationHessianImplementation(const LinearCombinationHessianImplementation & other) { return new OT::LinearCombinationHessianImplementation(other); } } }
