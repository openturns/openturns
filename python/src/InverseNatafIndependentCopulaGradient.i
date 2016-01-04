// SWIG file InverseNatafIndependentCopulaGradient.i

OTAutodoc(InverseNatafIndependentCopulaGradient)

%{
#include "InverseNatafIndependentCopulaGradient.hxx"
%}

%include InverseNatafIndependentCopulaGradient.hxx
namespace OT { %extend InverseNatafIndependentCopulaGradient { InverseNatafIndependentCopulaGradient(const InverseNatafIndependentCopulaGradient & other) { return new OT::InverseNatafIndependentCopulaGradient(other); } } }
