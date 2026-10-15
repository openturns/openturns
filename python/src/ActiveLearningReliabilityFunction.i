// SWIG file ActiveLearningReliabilityFunction.i

%{
#include "openturns/ActiveLearningReliabilityFunction.hxx"
%}

%include ActiveLearningReliabilityFunction_doc.i

OTTypedInterfaceObjectHelper(ActiveLearningReliabilityFunction)

%copyctor OT::ActiveLearningReliabilityFunction;

%include openturns/ActiveLearningReliabilityFunction.hxx
