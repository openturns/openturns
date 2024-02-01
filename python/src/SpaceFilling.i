// SWIG file

%{
#include "openturns/SpaceFilling.hxx"
%}

%include SpaceFilling_doc.i

OTTypedInterfaceObjectHelper(SpaceFilling)

%copyctor OT::SpaceFilling;

%include openturns/SpaceFilling.hxx
