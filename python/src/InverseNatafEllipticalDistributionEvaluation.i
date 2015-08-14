// SWIG file InverseNatafEllipticalDistributionEvaluation.i

%{
#include "InverseNatafEllipticalDistributionEvaluation.hxx"
%}

%include InverseNatafEllipticalDistributionEvaluation.hxx
namespace OT { %extend InverseNatafEllipticalDistributionEvaluation { InverseNatafEllipticalDistributionEvaluation(const InverseNatafEllipticalDistributionEvaluation & other) { return new OT::InverseNatafEllipticalDistributionEvaluation(other); } } }
