// SWIG file TrendEvaluationImplementation.i

%{
#include "TrendEvaluationImplementation.hxx"
%}

%include TrendEvaluationImplementation.hxx
namespace OT { %extend TrendEvaluationImplementation { TrendEvaluationImplementation(const TrendEvaluationImplementation & other) { return new OT::TrendEvaluationImplementation(other); } } }
