// SWIG file ExpectationSimulationAlgorithm.i

%{
#include "openturns/ExpectationSimulationAlgorithm.hxx"
%}

%include ExpectationSimulationAlgorithm_doc.i

%include openturns/ExpectationSimulationAlgorithm.hxx

namespace OT {

%extend ExpectationSimulationAlgorithm {

ExpectationSimulationAlgorithm(const ExpectationSimulationAlgorithm & other) { return new OT::ExpectationSimulationAlgorithm(other); }

} // ExpectationSimulationAlgorithm
} // OT
