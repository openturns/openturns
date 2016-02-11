// SWIG file IntegrationAlgorithm.i

OTAutodoc(IntegrationAlgorithm)

%{
#include "openturns/IntegrationAlgorithm.hxx"
%}

OTTypedInterfaceObjectHelper(IntegrationAlgorithm)

%include openturns/IntegrationAlgorithm.hxx
namespace OT{ %extend IntegrationAlgorithm { IntegrationAlgorithm(const IntegrationAlgorithm & other) { return new OT::IntegrationAlgorithm(other); } } }
