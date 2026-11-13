// SWIG file QuantileSimulationResult.i

%{
#include "openturns/QuantileSimulationResult.hxx"
%}

%include QuantileSimulationResult_doc.i

OTTypedInterfaceObjectHelper(QuantileSimulationResult)

OTDefaultCollectionConvertFunctions(QuantileSimulationResult)
OTTypedCollectionInterfaceObjectHelper(QuantileSimulationResult)

%copyctor OT::QuantileSimulationResult;

%include openturns/QuantileSimulationResult.hxx
