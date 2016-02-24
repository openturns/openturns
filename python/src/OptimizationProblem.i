// SWIG file OptimizationProblem.i

%{
#include "openturns/OptimizationProblem.hxx"
%}

%include OptimizationProblem_doc.i

%template(OptimizationProblemImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::OptimizationProblemImplementation>;

%include openturns/OptimizationProblem.hxx
namespace OT{ %extend OptimizationProblem { OptimizationProblem(const OptimizationProblem & other) { return new OT::OptimizationProblem(other); } } }
