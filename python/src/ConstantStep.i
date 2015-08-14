// SWIG file ConstantStep.i

%{
#include "ConstantStep.hxx"
%}

%include ConstantStep_doc.i

%include ConstantStep.hxx
namespace OT { %extend ConstantStep { ConstantStep(const ConstantStep & other) { return new OT::ConstantStep(other); } } }
