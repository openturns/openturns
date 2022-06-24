// SWIG file ProjectionStrategy.i

%{
#include "openturns/ProjectionStrategy.hxx"
%}

%include ProjectionStrategy_doc.i

OTTypedInterfaceObjectHelper(ProjectionStrategy)

%ignore OT::ProjectionStrategy::computeCoefficients;

%include openturns/ProjectionStrategy.hxx
namespace OT{ %extend ProjectionStrategy { ProjectionStrategy(const ProjectionStrategy & other) { return new OT::ProjectionStrategy(other); } } }
