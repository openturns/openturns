// SWIG file NatafEllipticalDistributionGradient.i

OTAutodoc(NatafEllipticalDistributionGradient)

%{
#include "openturns/NatafEllipticalDistributionGradient.hxx"
%}

%include openturns/NatafEllipticalDistributionGradient.hxx
namespace OT { %extend NatafEllipticalDistributionGradient { NatafEllipticalDistributionGradient(const NatafEllipticalDistributionGradient & other) { return new OT::NatafEllipticalDistributionGradient(other); } } }
