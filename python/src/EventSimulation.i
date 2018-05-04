// SWIG file EventSimulation.i

%{
#include "openturns/EventSimulation.hxx"
%}

%include EventSimulation_doc.i

%include openturns/EventSimulation.hxx

namespace OT {

%extend EventSimulation {

EventSimulation(const EventSimulation & other) { return new OT::EventSimulation(other); }

} // EventSimulation
} // OT
