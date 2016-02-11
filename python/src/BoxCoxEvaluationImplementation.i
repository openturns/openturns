// SWIG file BoxCoxEvaluationImplementation.i

OTAutodoc(BoxCoxEvaluationImplementation)

%{
#include "openturns/BoxCoxEvaluationImplementation.hxx"
%}

%include openturns/BoxCoxEvaluationImplementation.hxx
namespace OT { %extend BoxCoxEvaluationImplementation { BoxCoxEvaluationImplementation(const BoxCoxEvaluationImplementation & other) { return new OT::BoxCoxEvaluationImplementation(other); } } }
