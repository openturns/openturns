// SWIG file NatafIndependentCopulaGradient.i

%{
#include "NatafIndependentCopulaGradient.hxx"
%}

%include NatafIndependentCopulaGradient.hxx
namespace OT { %extend NatafIndependentCopulaGradient { NatafIndependentCopulaGradient(const NatafIndependentCopulaGradient & other) { return new OT::NatafIndependentCopulaGradient(other); } } }
