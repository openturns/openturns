// SWIG file NatafEllipticalCopulaHessian.i

%{
#include "NatafEllipticalCopulaHessian.hxx"
%}

%include NatafEllipticalCopulaHessian.hxx
namespace OT { %extend NatafEllipticalCopulaHessian { NatafEllipticalCopulaHessian(const NatafEllipticalCopulaHessian & other) { return new OT::NatafEllipticalCopulaHessian(other); } } }
