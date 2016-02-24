// SWIG file LinearCombinationEvaluationImplementation.i

OTAutodoc(LinearCombinationEvaluationImplementation)

%{
#include "openturns/LinearCombinationEvaluationImplementation.hxx"
%}

%include openturns/LinearCombinationEvaluationImplementation.hxx

namespace OT { %extend LinearCombinationEvaluationImplementation { LinearCombinationEvaluationImplementation(const LinearCombinationEvaluationImplementation & other) { return new OT::LinearCombinationEvaluationImplementation(other); } } }
