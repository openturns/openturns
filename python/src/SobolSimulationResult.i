// SWIG file SobolSimulationResult.i

%{
#include "openturns/SobolSimulationResult.hxx"
%}

%include SobolSimulationResult_doc.i

%include openturns/SobolSimulationResult.hxx
namespace OT{ %extend SobolSimulationResult { SobolSimulationResult(const SobolSimulationResult & other) { return new OT::SobolSimulationResult(other); } } }
