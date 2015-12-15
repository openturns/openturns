// SWIG file InverseNatafIndependentCopulaEvaluation.i

OTAutodoc(InverseNatafIndependentCopulaEvaluation)

%{
#include "InverseNatafIndependentCopulaEvaluation.hxx"
%}

%include InverseNatafIndependentCopulaEvaluation.hxx
namespace OT { %extend InverseNatafIndependentCopulaEvaluation { InverseNatafIndependentCopulaEvaluation(const InverseNatafIndependentCopulaEvaluation & other) { return new OT::InverseNatafIndependentCopulaEvaluation(other); } } }
