// SWIG file InverseNatafEllipticalDistributionGradient.i

OTAutodoc(InverseNatafEllipticalDistributionGradient)

%{
#include "openturns/InverseNatafEllipticalDistributionGradient.hxx"
%}

%include openturns/InverseNatafEllipticalDistributionGradient.hxx
namespace OT { %extend InverseNatafEllipticalDistributionGradient { InverseNatafEllipticalDistributionGradient(const InverseNatafEllipticalDistributionGradient & other) { return new OT::InverseNatafEllipticalDistributionGradient(other); } } }
