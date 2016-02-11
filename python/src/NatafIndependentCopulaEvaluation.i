// SWIG file NatafIndependentCopulaEvaluation.i

OTAutodoc(NatafIndependentCopulaEvaluation)

%{
#include "openturns/NatafIndependentCopulaEvaluation.hxx"
%}

%include openturns/NatafIndependentCopulaEvaluation.hxx
namespace OT { %extend NatafIndependentCopulaEvaluation { NatafIndependentCopulaEvaluation(const NatafIndependentCopulaEvaluation & other) { return new OT::NatafIndependentCopulaEvaluation(other); } } }
