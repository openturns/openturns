// SWIG file ConstantStep.i

%{
#include "openturns/ConstantStep.hxx"
%}

%include ConstantStep_doc.i

%include openturns/ConstantStep.hxx
namespace OT { %extend ConstantStep { ConstantStep(const ConstantStep & other) { return new OT::ConstantStep(other); } } }
