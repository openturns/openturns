// SWIG file LowDiscrepancySequenceImplementation.i

%{
#include "LowDiscrepancySequenceImplementation.hxx"
%}

%include LowDiscrepancySequenceImplementation_doc.i

%include LowDiscrepancySequenceImplementation.hxx
namespace OT{ %extend LowDiscrepancySequenceImplementation { LowDiscrepancySequenceImplementation(const LowDiscrepancySequenceImplementation & other) { return new OT::LowDiscrepancySequenceImplementation(other); } } }
