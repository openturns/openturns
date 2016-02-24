// SWIG file ProjectionStrategyImplementation.i

%{
#include "openturns/ProjectionStrategyImplementation.hxx"
%}

%include ProjectionStrategyImplementation_doc.i

%include openturns/ProjectionStrategyImplementation.hxx
namespace OT{ %extend ProjectionStrategyImplementation { ProjectionStrategyImplementation(const ProjectionStrategyImplementation & other) { return new OT::ProjectionStrategyImplementation(other); } } }
