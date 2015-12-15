// SWIG file InverseNatafEllipticalDistributionHessian.i

OTAutodoc(InverseNatafEllipticalDistributionHessian)

%{
#include "InverseNatafEllipticalDistributionHessian.hxx"
%}

%include InverseNatafEllipticalDistributionHessian.hxx
namespace OT { %extend InverseNatafEllipticalDistributionHessian { InverseNatafEllipticalDistributionHessian(const InverseNatafEllipticalDistributionHessian & other) { return new OT::InverseNatafEllipticalDistributionHessian(other); } } }
