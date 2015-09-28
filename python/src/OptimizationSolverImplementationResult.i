// SWIG file OptimizationSolverImplementationResult.i

%{
#include "OptimizationSolverImplementationResult.hxx"
%}

%include OptimizationSolverImplementationResult_doc.i

%include OptimizationSolverImplementationResult.hxx
namespace OT{ %extend OptimizationSolverImplementationResult { OptimizationSolverImplementationResult(const OptimizationSolverImplementationResult & other) { return new OT::OptimizationSolverImplementationResult(other); } } }
