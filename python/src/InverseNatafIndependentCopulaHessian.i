// SWIG file InverseNatafIndependentCopulaHessian.i

OTAutodoc(InverseNatafIndependentCopulaHessian)

%{
#include "InverseNatafIndependentCopulaHessian.hxx"
%}

%include InverseNatafIndependentCopulaHessian.hxx
namespace OT { %extend InverseNatafIndependentCopulaHessian { InverseNatafIndependentCopulaHessian(const InverseNatafIndependentCopulaHessian & other) { return new OT::InverseNatafIndependentCopulaHessian(other); } } }
