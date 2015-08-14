// SWIG file NatafIndependentCopulaEvaluation.i

%{
#include "NatafIndependentCopulaEvaluation.hxx"
%}

%include NatafIndependentCopulaEvaluation.hxx
namespace OT { %extend NatafIndependentCopulaEvaluation { NatafIndependentCopulaEvaluation(const NatafIndependentCopulaEvaluation & other) { return new OT::NatafIndependentCopulaEvaluation(other); } } }
