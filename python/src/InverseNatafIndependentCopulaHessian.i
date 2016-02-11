// SWIG file InverseNatafIndependentCopulaHessian.i

OTAutodoc(InverseNatafIndependentCopulaHessian)

%{
#include "openturns/InverseNatafIndependentCopulaHessian.hxx"
%}

%include openturns/InverseNatafIndependentCopulaHessian.hxx
namespace OT { %extend InverseNatafIndependentCopulaHessian { InverseNatafIndependentCopulaHessian(const InverseNatafIndependentCopulaHessian & other) { return new OT::InverseNatafIndependentCopulaHessian(other); } } }
