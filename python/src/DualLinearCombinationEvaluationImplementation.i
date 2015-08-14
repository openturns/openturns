// SWIG file DualLinearCombinationEvaluationImplementation.i

%{
#include "DualLinearCombinationEvaluationImplementation.hxx"
%}

%include DualLinearCombinationEvaluationImplementation_doc.i

%include DualLinearCombinationEvaluationImplementation.hxx

namespace OT { %extend DualLinearCombinationEvaluationImplementation { DualLinearCombinationEvaluationImplementation(const DualLinearCombinationEvaluationImplementation & other) { return new OT::DualLinearCombinationEvaluationImplementation(other); } } }
