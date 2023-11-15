// SWIG file FiniteDifferenceStep.i

%{
#include "openturns/FiniteDifferenceStep.hxx"
%}

%include FiniteDifferenceStep_doc.i

OTTypedInterfaceObjectHelper(FiniteDifferenceStep)

%copyctor OT::FiniteDifferenceStep;

%include openturns/FiniteDifferenceStep.hxx
