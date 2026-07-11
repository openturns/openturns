// SWIG file SobolSimulationResult.i

%{
#include "openturns/SobolSimulationResult.hxx"
%}

%include SobolSimulationResult_doc.i

%copyctor OT::SobolSimulationResult;

%include openturns/SobolSimulationResult.hxx

namespace OT {

%extend SobolSimulationResult {
  SobolSimulationResult(const SimulationResult & result) {
    return new OT::SobolSimulationResult(dynamic_cast<const OT::SobolSimulationResult &>(*result.getImplementation()));
  }
}

} // namespace OT
