// SWIG file OptimizationAlgorithm.i

%{
#include "openturns/OptimizationAlgorithm.hxx"
%}

%include OptimizationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(OptimizationAlgorithm)

%include openturns/OptimizationAlgorithm.hxx
namespace OT{ %extend OptimizationAlgorithm { OptimizationAlgorithm(const OptimizationAlgorithm & other) { return new OT::OptimizationAlgorithm(other); } } }
