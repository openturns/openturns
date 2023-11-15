// SWIG file IntegrationAlgorithm.i

%{
#include "openturns/IntegrationAlgorithm.hxx"
%}

%include IntegrationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(IntegrationAlgorithm)

%copyctor OT::IntegrationAlgorithm;

%include openturns/IntegrationAlgorithm.hxx
