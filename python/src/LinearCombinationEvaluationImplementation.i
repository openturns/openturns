// SWIG file LinearCombinationEvaluationImplementation.i

%{
#include "LinearCombinationEvaluationImplementation.hxx"
%}

%include LinearCombinationEvaluationImplementation.hxx

namespace OT { %extend LinearCombinationEvaluationImplementation { LinearCombinationEvaluationImplementation(const LinearCombinationEvaluationImplementation & other) { return new OT::LinearCombinationEvaluationImplementation(other); } } }
