// SWIG file DistributionParameters.i

%{
#include "DistributionParameters.hxx"
%}

%include DistributionParameters_doc.i

OTTypedInterfaceObjectHelper(DistributionParameters)

%include DistributionParameters.hxx
namespace OT { %extend DistributionParameters { DistributionParameters(const DistributionParameters & other) { return new OT::DistributionParameters(other); } } }
