// SWIG file SimulationResultImplementation.i

%{
#include "openturns/SimulationResultImplementation.hxx"
%}

%include SimulationResultImplementation_doc.i

%include openturns/SimulationResultImplementation.hxx
namespace OT{ %extend SimulationResultImplementation { SimulationResultImplementation(const SimulationResultImplementation & other) { return new OT::SimulationResultImplementation(other); } } }
