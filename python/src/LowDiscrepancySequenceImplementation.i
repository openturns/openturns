// SWIG file LowDiscrepancySequenceImplementation.i

%{
#include "openturns/LowDiscrepancySequenceImplementation.hxx"
%}

%include LowDiscrepancySequenceImplementation_doc.i

%include openturns/LowDiscrepancySequenceImplementation.hxx
namespace OT{ %extend LowDiscrepancySequenceImplementation { LowDiscrepancySequenceImplementation(const LowDiscrepancySequenceImplementation & other) { return new OT::LowDiscrepancySequenceImplementation(other); } } }
