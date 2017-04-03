// SWIG file ComposedGradient.i

OTAutodoc(ComposedGradient)

%{
#include "openturns/ComposedGradient.hxx"
%}

%include openturns/ComposedGradient.hxx
namespace OT { %extend ComposedGradient { ComposedGradient(const ComposedGradient & other) { return new OT::ComposedGradient(other); } } }
