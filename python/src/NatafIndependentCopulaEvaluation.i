// SWIG file NatafIndependentCopulaEvaluation.i

OTAutodoc(NatafIndependentCopulaEvaluation)

%{
#include "NatafIndependentCopulaEvaluation.hxx"
%}

%include NatafIndependentCopulaEvaluation.hxx
namespace OT { %extend NatafIndependentCopulaEvaluation { NatafIndependentCopulaEvaluation(const NatafIndependentCopulaEvaluation & other) { return new OT::NatafIndependentCopulaEvaluation(other); } } }
