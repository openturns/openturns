// SWIG file OptimizationSolverImplementation.i

%{
#include "OptimizationSolverImplementation.hxx"
%}

%include OptimizationSolverImplementation.hxx
namespace OT{ %extend OptimizationSolverImplementation { OptimizationSolverImplementation(const OptimizationSolverImplementation & other) { return new OT::OptimizationSolverImplementation(other); } } }
