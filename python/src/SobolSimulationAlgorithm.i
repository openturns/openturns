// SWIG file SobolSimulationAlgorithm.i

%{
#include "openturns/SobolSimulationAlgorithm.hxx"
%}

%include SobolSimulationAlgorithm_doc.i

%include openturns/SobolSimulationAlgorithm.hxx

namespace OT {

%extend SobolSimulationAlgorithm {

SobolSimulationAlgorithm(const SobolSimulationAlgorithm & other) { return new OT::SobolSimulationAlgorithm(other); }

} // SobolSimulationAlgorithm
} // OT
