// SWIG file BlendedStep.i

%{
#include "BlendedStep.hxx"
%}

%include BlendedStep_doc.i

%include BlendedStep.hxx
namespace OT { %extend BlendedStep { BlendedStep(const BlendedStep & other) { return new OT::BlendedStep(other); } } }
