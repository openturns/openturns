// SWIG file InverseNatafEllipticalCopulaHessian.i

OTAutodoc(InverseNatafEllipticalCopulaHessian)

%{
#include "openturns/InverseNatafEllipticalCopulaHessian.hxx"
%}

%include openturns/InverseNatafEllipticalCopulaHessian.hxx
namespace OT { %extend InverseNatafEllipticalCopulaHessian { InverseNatafEllipticalCopulaHessian(const InverseNatafEllipticalCopulaHessian & other) { return new OT::InverseNatafEllipticalCopulaHessian(other); } } }
