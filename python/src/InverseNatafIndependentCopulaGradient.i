// SWIG file InverseNatafIndependentCopulaGradient.i

OTAutodoc(InverseNatafIndependentCopulaGradient)

%{
#include "openturns/InverseNatafIndependentCopulaGradient.hxx"
%}

%include openturns/InverseNatafIndependentCopulaGradient.hxx
namespace OT { %extend InverseNatafIndependentCopulaGradient { InverseNatafIndependentCopulaGradient(const InverseNatafIndependentCopulaGradient & other) { return new OT::InverseNatafIndependentCopulaGradient(other); } } }
