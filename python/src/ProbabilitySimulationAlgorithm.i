// SWIG file ProbabilitySimulationAlgorithm.i

%{
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
%}

%include ProbabilitySimulationAlgorithm_doc.i

%include openturns/ProbabilitySimulationAlgorithm.hxx

namespace OT {


%extend ProbabilitySimulationAlgorithm {

ProbabilitySimulationAlgorithm(const ProbabilitySimulationAlgorithm & other) { return new OT::ProbabilitySimulationAlgorithm(other); }

} // ProbabilitySimulationAlgorithm
} // OT

