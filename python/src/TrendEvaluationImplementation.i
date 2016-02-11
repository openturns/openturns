// SWIG file TrendEvaluationImplementation.i

OTAutodoc(TrendEvaluationImplementation)

%{
#include "openturns/TrendEvaluationImplementation.hxx"
%}

%include openturns/TrendEvaluationImplementation.hxx
namespace OT { %extend TrendEvaluationImplementation { TrendEvaluationImplementation(const TrendEvaluationImplementation & other) { return new OT::TrendEvaluationImplementation(other); } } }
