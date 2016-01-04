// SWIG file LinearCombinationHessianImplementation.i

OTAutodoc(LinearCombinationHessianImplementation)

%{
#include "LinearCombinationHessianImplementation.hxx"
%}

%include LinearCombinationHessianImplementation.hxx
namespace OT { %extend LinearCombinationHessianImplementation { LinearCombinationHessianImplementation(const LinearCombinationHessianImplementation & other) { return new OT::LinearCombinationHessianImplementation(other); } } }
