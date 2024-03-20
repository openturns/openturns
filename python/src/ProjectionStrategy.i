// SWIG file ProjectionStrategy.i

%{
#include "openturns/ProjectionStrategy.hxx"
%}

%include ProjectionStrategy_doc.i

OTTypedInterfaceObjectHelper(ProjectionStrategy)

%copyctor OT::ProjectionStrategy;

%include openturns/ProjectionStrategy.hxx
