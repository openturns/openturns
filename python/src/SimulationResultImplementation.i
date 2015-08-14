// SWIG file SimulationResultImplementation.i

%{
#include "SimulationResultImplementation.hxx"
%}

%include SimulationResultImplementation_doc.i

%include SimulationResultImplementation.hxx
namespace OT{ %extend SimulationResultImplementation { SimulationResultImplementation(const SimulationResultImplementation & other) { return new OT::SimulationResultImplementation(other); } } }
