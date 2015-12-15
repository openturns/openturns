// SWIG file InverseTrendEvaluationImplementation.i

OTAutodoc(InverseTrendEvaluationImplementation)

%{
#include "InverseTrendEvaluationImplementation.hxx"
%}

%include InverseTrendEvaluationImplementation.hxx
namespace OT { %extend InverseTrendEvaluationImplementation { InverseTrendEvaluationImplementation(const InverseTrendEvaluationImplementation & other) { return new OT::InverseTrendEvaluationImplementation(other); } } }
