// SWIG file OptimizationProblem.i

%{
#include "openturns/OptimizationProblem.hxx"
%}

%include OptimizationProblem_doc.i

OTTypedInterfaceObjectHelper(OptimizationProblem)

%include openturns/OptimizationProblem.hxx
namespace OT{ %extend OptimizationProblem { OptimizationProblem(const OptimizationProblem & other) { return new OT::OptimizationProblem(other); } } }
