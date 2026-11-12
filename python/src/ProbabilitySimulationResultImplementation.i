// SWIG file ProbabilitySimulationResultImplementation.i

%{
#include "openturns/ProbabilitySimulationResultImplementation.hxx"
%}

%include ProbabilitySimulationResultImplementation_doc.i

%copyctor OT::ProbabilitySimulationResultImplementation;

%ignore OT::ProbabilitySimulationResultImplementation::GetDefaultConfidenceLevel;

%include openturns/ProbabilitySimulationResultImplementation.hxx
