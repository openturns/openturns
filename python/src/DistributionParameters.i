// SWIG file DistributionParameters.i

%{
#include "openturns/DistributionParameters.hxx"
%}

%include DistributionParameters_doc.i

OTTypedInterfaceObjectHelper(DistributionParameters)

%copyctor OT::DistributionParameters;

%include openturns/DistributionParameters.hxx
