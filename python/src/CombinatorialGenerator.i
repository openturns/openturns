// SWIG file CombinatorialGenerator.i

%{
#include "CombinatorialGenerator.hxx"
%}

OTTypedInterfaceObjectHelper(CombinatorialGenerator)

%include CombinatorialGenerator.hxx
namespace OT { %extend CombinatorialGenerator { CombinatorialGenerator(const CombinatorialGenerator & other) { return new OT::CombinatorialGenerator(other); } } }
