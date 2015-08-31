// SWIG file OptimizationProblemImplementation.i

%{
#include "OptimizationProblemImplementation.hxx"
%}

%include OptimizationProblemImplementation_doc.i

%include OptimizationProblemImplementation.hxx
namespace OT{ %extend OptimizationProblemImplementation { OptimizationProblemImplementation(const OptimizationProblemImplementation & other) { return new OT::OptimizationProblemImplementation(other); } } }
