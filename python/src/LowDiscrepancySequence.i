// SWIG file LowDiscrepancySequence.i

%{
#include "openturns/LowDiscrepancySequence.hxx"
%}

%include LowDiscrepancySequence_doc.i

OTTypedInterfaceObjectHelper(LowDiscrepancySequence)

%copyctor OT::LowDiscrepancySequence;

%include openturns/LowDiscrepancySequence.hxx
