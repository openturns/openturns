// SWIG file NatafIndependentCopulaGradient.i

OTAutodoc(NatafIndependentCopulaGradient)

%{
#include "openturns/NatafIndependentCopulaGradient.hxx"
%}

%include openturns/NatafIndependentCopulaGradient.hxx
namespace OT { %extend NatafIndependentCopulaGradient { NatafIndependentCopulaGradient(const NatafIndependentCopulaGradient & other) { return new OT::NatafIndependentCopulaGradient(other); } } }
