// SWIG file FiniteDifferenceStep.i

%{
#include "openturns/FiniteDifferenceStep.hxx"
%}

%include FiniteDifferenceStep_doc.i

OTTypedInterfaceObjectHelper(FiniteDifferenceStep)

%include openturns/FiniteDifferenceStep.hxx
namespace OT { %extend FiniteDifferenceStep { FiniteDifferenceStep(const FiniteDifferenceStep & other) { return new OT::FiniteDifferenceStep(other); } } }
