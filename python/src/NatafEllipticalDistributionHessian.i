// SWIG file NatafEllipticalDistributionHessian.i

OTAutodoc(NatafEllipticalDistributionHessian)

%{
#include "openturns/NatafEllipticalDistributionHessian.hxx"
%}

%include openturns/NatafEllipticalDistributionHessian.hxx
namespace OT { %extend NatafEllipticalDistributionHessian { NatafEllipticalDistributionHessian(const NatafEllipticalDistributionHessian & other) { return new OT::NatafEllipticalDistributionHessian(other); } } }
