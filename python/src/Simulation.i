// SWIG file Simulation.i

%{
#include "openturns/Simulation.hxx"
%}

%include Simulation_doc.i

%include openturns/Simulation.hxx

namespace OT {

%extend Simulation {

Simulation(const Simulation & other) { return new OT::Simulation(other); }

} // Simulation
} // OT
