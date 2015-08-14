// SWIG file SimulationResult.i

%{
#include "SimulationResult.hxx"
%}

%include SimulationResult_doc.i

OTTypedInterfaceObjectHelper(SimulationResult)

%include SimulationResult.hxx
namespace OT{ %extend SimulationResult { SimulationResult(const SimulationResult & other) { return new OT::SimulationResult(other); } } }
