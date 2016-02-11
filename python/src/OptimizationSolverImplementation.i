// SWIG file OptimizationSolverImplementation.i

%{
#include "openturns/OptimizationSolverImplementation.hxx"
%}

%include OptimizationSolverImplementation_doc.i

%include openturns/OptimizationSolverImplementation.hxx
namespace OT{ %extend OptimizationSolverImplementation { OptimizationSolverImplementation(const OptimizationSolverImplementation & other) { return new OT::OptimizationSolverImplementation(other); } } }
