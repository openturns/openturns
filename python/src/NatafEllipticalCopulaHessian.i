// SWIG file NatafEllipticalCopulaHessian.i

OTAutodoc(NatafEllipticalCopulaHessian)

%{
#include "openturns/NatafEllipticalCopulaHessian.hxx"
%}

%include openturns/NatafEllipticalCopulaHessian.hxx
namespace OT { %extend NatafEllipticalCopulaHessian { NatafEllipticalCopulaHessian(const NatafEllipticalCopulaHessian & other) { return new OT::NatafEllipticalCopulaHessian(other); } } }
