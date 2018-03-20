// SWIG file ProbabilitySimulationResult.i

%{
#include "openturns/ProbabilitySimulationResult.hxx"
%}

%include ProbabilitySimulationResult_doc.i

%include openturns/ProbabilitySimulationResult.hxx
namespace OT{ %extend ProbabilitySimulationResult { ProbabilitySimulationResult(const ProbabilitySimulationResult & other) { return new OT::ProbabilitySimulationResult(other); } } }
