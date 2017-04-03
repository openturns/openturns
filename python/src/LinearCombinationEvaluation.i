// SWIG file LinearCombinationEvaluation.i

OTAutodoc(LinearCombinationEvaluation)

%{
#include "openturns/LinearCombinationEvaluation.hxx"
%}

%include openturns/LinearCombinationEvaluation.hxx

namespace OT { %extend LinearCombinationEvaluation { LinearCombinationEvaluation(const LinearCombinationEvaluation & other) { return new OT::LinearCombinationEvaluation(other); } } }
