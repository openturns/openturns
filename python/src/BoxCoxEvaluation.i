// SWIG file BoxCoxEvaluation.i

OTAutodoc(BoxCoxEvaluation)

%{
#include "openturns/BoxCoxEvaluation.hxx"
%}

%include openturns/BoxCoxEvaluation.hxx
namespace OT { %extend BoxCoxEvaluation { BoxCoxEvaluation(const BoxCoxEvaluation & other) { return new OT::BoxCoxEvaluation(other); } } }
