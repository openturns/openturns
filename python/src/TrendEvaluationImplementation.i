// SWIG file TrendEvaluationImplementation.i

OTAutodoc(TrendEvaluationImplementation)

%{
#include "TrendEvaluationImplementation.hxx"
%}

%include TrendEvaluationImplementation.hxx
namespace OT { %extend TrendEvaluationImplementation { TrendEvaluationImplementation(const TrendEvaluationImplementation & other) { return new OT::TrendEvaluationImplementation(other); } } }
