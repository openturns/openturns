// SWIG file NatafEllipticalDistributionEvaluation.i

OTAutodoc(NatafEllipticalDistributionEvaluation)

%{
#include "openturns/NatafEllipticalDistributionEvaluation.hxx"
%}

%include openturns/NatafEllipticalDistributionEvaluation.hxx
namespace OT { %extend NatafEllipticalDistributionEvaluation { NatafEllipticalDistributionEvaluation(const NatafEllipticalDistributionEvaluation & other) { return new OT::NatafEllipticalDistributionEvaluation(other); } } }
