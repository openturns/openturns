// SWIG file InverseTrendEvaluationImplementation.i

OTAutodoc(InverseTrendEvaluationImplementation)

%{
#include "openturns/InverseTrendEvaluationImplementation.hxx"
%}

%include openturns/InverseTrendEvaluationImplementation.hxx
namespace OT { %extend InverseTrendEvaluationImplementation { InverseTrendEvaluationImplementation(const InverseTrendEvaluationImplementation & other) { return new OT::InverseTrendEvaluationImplementation(other); } } }
