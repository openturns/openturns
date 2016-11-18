// SWIG file IntegrationAlgorithm.i

%{
#include "openturns/IntegrationAlgorithm.hxx"
%}

%include IntegrationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(IntegrationAlgorithm)

%include openturns/IntegrationAlgorithm.hxx
namespace OT{ %extend IntegrationAlgorithm { IntegrationAlgorithm(const IntegrationAlgorithm & other) { return new OT::IntegrationAlgorithm(other); } } }
