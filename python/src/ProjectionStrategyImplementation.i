// SWIG file ProjectionStrategyImplementation.i

%{
#include "openturns/ProjectionStrategyImplementation.hxx"
%}

%include ProjectionStrategyImplementation_doc.i

%ignore OT::ProjectionStrategyImplementation::getSelectionHistory;
%ignore OT::ProjectionStrategyImplementation::getErrorHistory;

%copyctor OT::ProjectionStrategyImplementation;

%include openturns/ProjectionStrategyImplementation.hxx
