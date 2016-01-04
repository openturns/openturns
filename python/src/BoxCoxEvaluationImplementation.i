// SWIG file BoxCoxEvaluationImplementation.i

OTAutodoc(BoxCoxEvaluationImplementation)

%{
#include "BoxCoxEvaluationImplementation.hxx"
%}

%include BoxCoxEvaluationImplementation.hxx
namespace OT { %extend BoxCoxEvaluationImplementation { BoxCoxEvaluationImplementation(const BoxCoxEvaluationImplementation & other) { return new OT::BoxCoxEvaluationImplementation(other); } } }
