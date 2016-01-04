// SWIG file NatafIndependentCopulaGradient.i

OTAutodoc(NatafIndependentCopulaGradient)

%{
#include "NatafIndependentCopulaGradient.hxx"
%}

%include NatafIndependentCopulaGradient.hxx
namespace OT { %extend NatafIndependentCopulaGradient { NatafIndependentCopulaGradient(const NatafIndependentCopulaGradient & other) { return new OT::NatafIndependentCopulaGradient(other); } } }
