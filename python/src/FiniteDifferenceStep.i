// SWIG file FiniteDifferenceStep.i

%{
#include "FiniteDifferenceStep.hxx"
%}

%include FiniteDifferenceStep_doc.i

OTTypedInterfaceObjectHelper(FiniteDifferenceStep)

%include FiniteDifferenceStep.hxx
namespace OT { %extend FiniteDifferenceStep { FiniteDifferenceStep(const FiniteDifferenceStep & other) { return new OT::FiniteDifferenceStep(other); } } }
