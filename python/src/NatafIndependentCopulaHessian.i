// SWIG file NatafIndependentCopulaHessian.i

OTAutodoc(NatafIndependentCopulaHessian)

%{
#include "NatafIndependentCopulaHessian.hxx"
%}

%include NatafIndependentCopulaHessian.hxx
namespace OT { %extend NatafIndependentCopulaHessian { NatafIndependentCopulaHessian(const NatafIndependentCopulaHessian & other) { return new OT::NatafIndependentCopulaHessian(other); } } }
