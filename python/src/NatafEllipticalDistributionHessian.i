// SWIG file NatafEllipticalDistributionHessian.i

OTAutodoc(NatafEllipticalDistributionHessian)

%{
#include "NatafEllipticalDistributionHessian.hxx"
%}

%include NatafEllipticalDistributionHessian.hxx
namespace OT { %extend NatafEllipticalDistributionHessian { NatafEllipticalDistributionHessian(const NatafEllipticalDistributionHessian & other) { return new OT::NatafEllipticalDistributionHessian(other); } } }
