// SWIG file IntegrationAlgorithm.i

%{
#include "IntegrationAlgorithm.hxx"
%}

OTTypedInterfaceObjectHelper(IntegrationAlgorithm)

%include IntegrationAlgorithm.hxx
namespace OT{ %extend IntegrationAlgorithm { IntegrationAlgorithm(const IntegrationAlgorithm & other) { return new OT::IntegrationAlgorithm(other); } } }
