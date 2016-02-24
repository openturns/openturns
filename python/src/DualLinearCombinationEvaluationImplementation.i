// SWIG file DualLinearCombinationEvaluationImplementation.i

%{
#include "openturns/DualLinearCombinationEvaluationImplementation.hxx"
%}

%include DualLinearCombinationEvaluationImplementation_doc.i

%include openturns/DualLinearCombinationEvaluationImplementation.hxx

namespace OT { %extend DualLinearCombinationEvaluationImplementation { DualLinearCombinationEvaluationImplementation(const DualLinearCombinationEvaluationImplementation & other) { return new OT::DualLinearCombinationEvaluationImplementation(other); } } }
