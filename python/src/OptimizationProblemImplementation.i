// SWIG file OptimizationProblemImplementation.i

%{
#include "OptimizationProblemImplementation.hxx"
%}

%include OptimizationProblemImplementation.hxx
namespace OT{ %extend OptimizationProblemImplementation { OptimizationProblemImplementation(const OptimizationProblemImplementation & other) { return new OT::OptimizationProblemImplementation(other); } } }
