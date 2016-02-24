// SWIG file InverseNatafEllipticalDistributionEvaluation.i

OTAutodoc(InverseNatafEllipticalDistributionEvaluation)

%{
#include "openturns/InverseNatafEllipticalDistributionEvaluation.hxx"
%}

%include openturns/InverseNatafEllipticalDistributionEvaluation.hxx
namespace OT { %extend InverseNatafEllipticalDistributionEvaluation { InverseNatafEllipticalDistributionEvaluation(const InverseNatafEllipticalDistributionEvaluation & other) { return new OT::InverseNatafEllipticalDistributionEvaluation(other); } } }
