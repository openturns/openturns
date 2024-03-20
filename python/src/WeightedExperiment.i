// SWIG file WeightedExperiment.i

%{
#include "openturns/WeightedExperiment.hxx"
%}

%include WeightedExperiment_doc.i

OTTypedInterfaceObjectHelper(WeightedExperiment)

OTDefaultCollectionConvertFunctions(WeightedExperiment)
OTTypedCollectionInterfaceObjectHelper(WeightedExperiment)

%copyctor OT::WeightedExperiment;

%include openturns/WeightedExperiment.hxx
