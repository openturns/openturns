// SWIG file DistributionParameters.i

%{
#include "openturns/DistributionParameters.hxx"
%}

%include DistributionParameters_doc.i

OTTypedInterfaceObjectHelper(DistributionParameters)

%include openturns/DistributionParameters.hxx
namespace OT { %extend DistributionParameters { DistributionParameters(const DistributionParameters & other) { return new OT::DistributionParameters(other); } } }
