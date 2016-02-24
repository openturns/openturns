// SWIG file InverseNatafIndependentCopulaEvaluation.i

OTAutodoc(InverseNatafIndependentCopulaEvaluation)

%{
#include "openturns/InverseNatafIndependentCopulaEvaluation.hxx"
%}

%include openturns/InverseNatafIndependentCopulaEvaluation.hxx
namespace OT { %extend InverseNatafIndependentCopulaEvaluation { InverseNatafIndependentCopulaEvaluation(const InverseNatafIndependentCopulaEvaluation & other) { return new OT::InverseNatafIndependentCopulaEvaluation(other); } } }
