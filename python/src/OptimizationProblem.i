// SWIG file OptimizationProblem.i

%{
#include "openturns/OptimizationProblem.hxx"
%}

%include OptimizationProblem_doc.i

OTTypedInterfaceObjectHelper(OptimizationProblem)

%copyctor OT::OptimizationProblem;

%include openturns/OptimizationProblem.hxx
