// SWIG file ExpectationSimulationResult.i

%{
#include "openturns/ExpectationSimulationResult.hxx"
%}

%include ExpectationSimulationResult_doc.i

%copyctor OT::ExpectationSimulationResult;

%include openturns/ExpectationSimulationResult.hxx

namespace OT {

%extend ExpectationSimulationResult {
  ExpectationSimulationResult(const SimulationResult & result) {
    return new OT::ExpectationSimulationResult(dynamic_cast<const OT::ExpectationSimulationResult &>(*result.getImplementation()));
  }
}

} // namespace OT
