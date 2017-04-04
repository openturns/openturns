// SWIG file AggregatedEvaluation.i

%{
#include "openturns/AggregatedEvaluation.hxx"
%}

%include AggregatedEvaluation_doc.i

%include openturns/AggregatedEvaluation.hxx

namespace OT { %extend AggregatedEvaluation { AggregatedEvaluation(const AggregatedEvaluation & other) { return new OT::AggregatedEvaluation(other); } } }
