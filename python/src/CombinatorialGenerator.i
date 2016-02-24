// SWIG file CombinatorialGenerator.i

%{
#include "openturns/CombinatorialGenerator.hxx"
%}

%include CombinatorialGenerator_doc.i

OTTypedInterfaceObjectHelper(CombinatorialGenerator)

%include openturns/CombinatorialGenerator.hxx
namespace OT { %extend CombinatorialGenerator { CombinatorialGenerator(const CombinatorialGenerator & other) { return new OT::CombinatorialGenerator(other); } } }
