// SWIG file LowDiscrepancySequence.i

%{
#include "openturns/LowDiscrepancySequence.hxx"
%}

%include LowDiscrepancySequence_doc.i

OTTypedInterfaceObjectHelper(LowDiscrepancySequence)

%include openturns/LowDiscrepancySequence.hxx
namespace OT{ %extend LowDiscrepancySequence { LowDiscrepancySequence(const LowDiscrepancySequence & other) { return new OT::LowDiscrepancySequence(other); } } }
