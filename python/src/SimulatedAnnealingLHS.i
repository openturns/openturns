// SWIG file

%{
#include "openturns/SimulatedAnnealingLHS.hxx"
%}

%include SimulatedAnnealingLHS_doc.i

%include openturns/SimulatedAnnealingLHS.hxx
namespace OT { %extend SimulatedAnnealingLHS { SimulatedAnnealingLHS(const SimulatedAnnealingLHS & other) { return new OT::SimulatedAnnealingLHS(other); } } }
