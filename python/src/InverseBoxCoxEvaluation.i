// SWIG file InverseBoxCoxEvaluation.i

OTAutodoc(InverseBoxCoxEvaluation)

%{
#include "openturns/InverseBoxCoxEvaluation.hxx"
%}

%include openturns/InverseBoxCoxEvaluation.hxx
namespace OT { %extend InverseBoxCoxEvaluation { InverseBoxCoxEvaluation(const InverseBoxCoxEvaluation & other) { return new OT::InverseBoxCoxEvaluation(other); } } }
