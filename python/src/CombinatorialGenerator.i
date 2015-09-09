// SWIG file CombinatorialGenerator.i

%{
#include "CombinatorialGenerator.hxx"
%}

%include CombinatorialGenerator_doc.i

OTTypedInterfaceObjectHelper(CombinatorialGenerator)

%include CombinatorialGenerator.hxx
namespace OT { %extend CombinatorialGenerator { CombinatorialGenerator(const CombinatorialGenerator & other) { return new OT::CombinatorialGenerator(other); } } }
