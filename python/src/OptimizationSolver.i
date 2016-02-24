// SWIG file OptimizationSolver.i

%{
#include "openturns/OptimizationSolver.hxx"
%}

%include OptimizationSolver_doc.i

OTTypedInterfaceObjectHelper(OptimizationSolver)

%include openturns/OptimizationSolver.hxx
namespace OT{ %extend OptimizationSolver { OptimizationSolver(const OptimizationSolver & other) { return new OT::OptimizationSolver(other); } } }
