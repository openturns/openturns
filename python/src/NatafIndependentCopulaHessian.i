// SWIG file NatafIndependentCopulaHessian.i

%{
#include "NatafIndependentCopulaHessian.hxx"
%}

%include NatafIndependentCopulaHessian.hxx
namespace OT { %extend NatafIndependentCopulaHessian { NatafIndependentCopulaHessian(const NatafIndependentCopulaHessian & other) { return new OT::NatafIndependentCopulaHessian(other); } } }
