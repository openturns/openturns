// SWIG file InverseNatafIndependentCopulaGradient.i

%{
#include "InverseNatafIndependentCopulaGradient.hxx"
%}

%include InverseNatafIndependentCopulaGradient.hxx
namespace OT { %extend InverseNatafIndependentCopulaGradient { InverseNatafIndependentCopulaGradient(const InverseNatafIndependentCopulaGradient & other) { return new OT::InverseNatafIndependentCopulaGradient(other); } } }
