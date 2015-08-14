// SWIG file ProjectionStrategy.i

%{
#include "ProjectionStrategy.hxx"
%}

%include ProjectionStrategy_doc.i

OTTypedInterfaceObjectHelper(ProjectionStrategy)

%include ProjectionStrategy.hxx
namespace OT{ %extend ProjectionStrategy { ProjectionStrategy(const ProjectionStrategy & other) { return new OT::ProjectionStrategy(other); } } }
