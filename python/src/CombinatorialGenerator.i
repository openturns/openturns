// SWIG file CombinatorialGenerator.i

%{
#include "openturns/CombinatorialGenerator.hxx"
%}

%include CombinatorialGenerator_doc.i

OTTypedInterfaceObjectHelper(CombinatorialGenerator)

%copyctor OT::CombinatorialGenerator;

%include openturns/CombinatorialGenerator.hxx
