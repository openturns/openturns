// SWIG file ComposedEvaluation.i

OTAutodoc(ComposedEvaluation)

%{
#include "openturns/ComposedEvaluation.hxx"
%}

%include openturns/ComposedEvaluation.hxx
namespace OT { %extend ComposedEvaluation { ComposedEvaluation(const ComposedEvaluation & other) { return new OT::ComposedEvaluation(other); } } }
