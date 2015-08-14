// SWIG file InverseNatafEllipticalDistributionGradient.i

%{
#include "InverseNatafEllipticalDistributionGradient.hxx"
%}

%include InverseNatafEllipticalDistributionGradient.hxx
namespace OT { %extend InverseNatafEllipticalDistributionGradient { InverseNatafEllipticalDistributionGradient(const InverseNatafEllipticalDistributionGradient & other) { return new OT::InverseNatafEllipticalDistributionGradient(other); } } }
