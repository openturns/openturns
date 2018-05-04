// SWIG file SimulationResult.i

%{
#include "openturns/SimulationResult.hxx"
%}

%include SimulationResult_doc.i

%include openturns/SimulationResult.hxx
namespace OT{ %extend SimulationResult { SimulationResult(const SimulationResult & other) { return new OT::SimulationResult(other); } } }
