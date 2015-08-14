// SWIG file LowDiscrepancySequence.i

%{
#include "LowDiscrepancySequence.hxx"
%}

%include LowDiscrepancySequence_doc.i

OTTypedInterfaceObjectHelper(LowDiscrepancySequence)

%include LowDiscrepancySequence.hxx
namespace OT{ %extend LowDiscrepancySequence { LowDiscrepancySequence(const LowDiscrepancySequence & other) { return new OT::LowDiscrepancySequence(other); } } }
