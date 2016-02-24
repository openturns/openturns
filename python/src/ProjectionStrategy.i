// SWIG file ProjectionStrategy.i

%{
#include "openturns/ProjectionStrategy.hxx"
%}

%include ProjectionStrategy_doc.i

OTTypedInterfaceObjectHelper(ProjectionStrategy)

%include openturns/ProjectionStrategy.hxx
namespace OT{ %extend ProjectionStrategy { ProjectionStrategy(const ProjectionStrategy & other) { return new OT::ProjectionStrategy(other); } } }
