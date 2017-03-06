// SWIG file OptimizationAlgorithmImplementation.i

%{
#include "openturns/OptimizationAlgorithmImplementation.hxx"
%}

%include OptimizationAlgorithmImplementation_doc.i

%include openturns/OptimizationAlgorithmImplementation.hxx
namespace OT{ %extend OptimizationAlgorithmImplementation { OptimizationAlgorithmImplementation(const OptimizationAlgorithmImplementation & other) { return new OT::OptimizationAlgorithmImplementation(other); } } }
