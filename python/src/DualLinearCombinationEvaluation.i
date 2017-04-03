// SWIG file DualLinearCombinationEvaluation.i

%{
#include "openturns/DualLinearCombinationEvaluation.hxx"
%}

%include DualLinearCombinationEvaluation_doc.i

%include openturns/DualLinearCombinationEvaluation.hxx

namespace OT { %extend DualLinearCombinationEvaluation { DualLinearCombinationEvaluation(const DualLinearCombinationEvaluation & other) { return new OT::DualLinearCombinationEvaluation(other); } } }
