// SWIG file InverseNatafIndependentCopulaHessian.i

%{
#include "InverseNatafIndependentCopulaHessian.hxx"
%}

%include InverseNatafIndependentCopulaHessian.hxx
namespace OT { %extend InverseNatafIndependentCopulaHessian { InverseNatafIndependentCopulaHessian(const InverseNatafIndependentCopulaHessian & other) { return new OT::InverseNatafIndependentCopulaHessian(other); } } }
