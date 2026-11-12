// SWIG file ActiveLearningReliabilityResult.i

%{
#include "openturns/ActiveLearningReliabilityResult.hxx"
%}

%include ActiveLearningReliabilityResult_doc.i

// History is filled by run(), not by users: hide the setter from Python
// (its const Collection& parameter would need per-module conversion
// machinery in every importing module); C++ keeps it.
%ignore OT::ActiveLearningReliabilityResult::setSimulationResults;

%copyctor OT::ActiveLearningReliabilityResult;

%include openturns/ActiveLearningReliabilityResult.hxx
