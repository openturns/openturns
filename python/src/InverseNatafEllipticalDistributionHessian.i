// SWIG file InverseNatafEllipticalDistributionHessian.i

OTAutodoc(InverseNatafEllipticalDistributionHessian)

%{
#include "openturns/InverseNatafEllipticalDistributionHessian.hxx"
%}

%include openturns/InverseNatafEllipticalDistributionHessian.hxx
namespace OT { %extend InverseNatafEllipticalDistributionHessian { InverseNatafEllipticalDistributionHessian(const InverseNatafEllipticalDistributionHessian & other) { return new OT::InverseNatafEllipticalDistributionHessian(other); } } }
