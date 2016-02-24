// SWIG file OptimizationProblemImplementation.i

%{
#include "openturns/OptimizationProblemImplementation.hxx"
%}

%include OptimizationProblemImplementation_doc.i

%include openturns/OptimizationProblemImplementation.hxx
namespace OT{ %extend OptimizationProblemImplementation { OptimizationProblemImplementation(const OptimizationProblemImplementation & other) { return new OT::OptimizationProblemImplementation(other); } } }
