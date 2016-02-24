// SWIG file BlendedStep.i

%{
#include "openturns/BlendedStep.hxx"
%}

%include BlendedStep_doc.i

%include openturns/BlendedStep.hxx
namespace OT { %extend BlendedStep { BlendedStep(const BlendedStep & other) { return new OT::BlendedStep(other); } } }
