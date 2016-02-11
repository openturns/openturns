// SWIG file InverseBoxCoxEvaluationImplementation.i

OTAutodoc(InverseBoxCoxEvaluationImplementation)

%{
#include "openturns/InverseBoxCoxEvaluationImplementation.hxx"
%}

%include openturns/InverseBoxCoxEvaluationImplementation.hxx
namespace OT { %extend InverseBoxCoxEvaluationImplementation { InverseBoxCoxEvaluationImplementation(const InverseBoxCoxEvaluationImplementation & other) { return new OT::InverseBoxCoxEvaluationImplementation(other); } } }
