// SWIG file ProjectionStrategyImplementation.i

%{
#include "ProjectionStrategyImplementation.hxx"
%}

%include ProjectionStrategyImplementation_doc.i

%include ProjectionStrategyImplementation.hxx
namespace OT{ %extend ProjectionStrategyImplementation { ProjectionStrategyImplementation(const ProjectionStrategyImplementation & other) { return new OT::ProjectionStrategyImplementation(other); } } }
