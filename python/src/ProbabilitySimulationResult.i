// SWIG file ProbabilitySimulationResult.i

%{
#include "openturns/ProbabilitySimulationResult.hxx"
%}

%include ProbabilitySimulationResult_doc.i

%copyctor OT::ProbabilitySimulationResult;

%include openturns/ProbabilitySimulationResult.hxx

namespace OT {

%extend ProbabilitySimulationResult {
  ProbabilitySimulationResult(const SimulationResult & result) {
    return new OT::ProbabilitySimulationResult(dynamic_cast<const OT::ProbabilitySimulationResult &>(*result.getImplementation()));
  }
}

} // namespace OT
