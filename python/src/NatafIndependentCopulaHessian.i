// SWIG file NatafIndependentCopulaHessian.i

OTAutodoc(NatafIndependentCopulaHessian)

%{
#include "openturns/NatafIndependentCopulaHessian.hxx"
%}

%include openturns/NatafIndependentCopulaHessian.hxx
namespace OT { %extend NatafIndependentCopulaHessian { NatafIndependentCopulaHessian(const NatafIndependentCopulaHessian & other) { return new OT::NatafIndependentCopulaHessian(other); } } }
