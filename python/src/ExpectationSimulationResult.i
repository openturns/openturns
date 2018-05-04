// SWIG file ExpectationSimulationResult.i

%{
#include "openturns/ExpectationSimulationResult.hxx"
%}

%include ExpectationSimulationResult_doc.i

%include openturns/ExpectationSimulationResult.hxx
namespace OT{ %extend ExpectationSimulationResult { ExpectationSimulationResult(const ExpectationSimulationResult & other) { return new OT::ExpectationSimulationResult(other); } } }
