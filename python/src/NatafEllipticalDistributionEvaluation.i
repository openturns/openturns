// SWIG file NatafEllipticalDistributionEvaluation.i

%{
#include "NatafEllipticalDistributionEvaluation.hxx"
%}

%include NatafEllipticalDistributionEvaluation.hxx
namespace OT { %extend NatafEllipticalDistributionEvaluation { NatafEllipticalDistributionEvaluation(const NatafEllipticalDistributionEvaluation & other) { return new OT::NatafEllipticalDistributionEvaluation(other); } } }
