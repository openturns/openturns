// SWIG file WeightedExperiment.i

%{
#include "openturns/WeightedExperiment.hxx"
%}

%include WeightedExperiment_doc.i

OTTypedInterfaceObjectHelper(WeightedExperiment)

OTDefaultCollectionConvertFunctions(WeightedExperiment)
OTTypedCollectionInterfaceObjectHelper(WeightedExperiment)

%include openturns/WeightedExperiment.hxx
namespace OT { %extend WeightedExperiment { WeightedExperiment(const WeightedExperiment & other) { return new OT::WeightedExperiment(other); } } }
