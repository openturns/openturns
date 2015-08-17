// SWIG file OptimizationSolver.i

%{
#include "OptimizationSolver.hxx"
%}

OTTypedInterfaceObjectHelper(OptimizationSolver)

%include OptimizationSolver.hxx
namespace OT{ %extend OptimizationSolver { OptimizationSolver(const OptimizationSolver & other) { return new OT::OptimizationSolver(other); } } }
