// SWIG file NoEvaluation.i

OTAutodoc(NoEvaluation)

%{
#include "openturns/NoEvaluation.hxx"
%}

%include openturns/NoEvaluation.hxx
namespace OT { %extend NoEvaluation { NoEvaluation(const NoEvaluation & other) { return new OT::NoEvaluation(other); } } }
