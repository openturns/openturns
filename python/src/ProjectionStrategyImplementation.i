// SWIG file ProjectionStrategyImplementation.i

%{
#include "openturns/ProjectionStrategyImplementation.hxx"
%}

%include ProjectionStrategyImplementation_doc.i

%ignore OT::ProjectionStrategyImplementation::getSelectionHistory;

%copyctor OT::ProjectionStrategyImplementation;

%include openturns/ProjectionStrategyImplementation.hxx
