// SWIG file

%{
#include "openturns/SpaceFilling.hxx"
%}

%include SpaceFilling_doc.i

OTTypedInterfaceObjectHelper(SpaceFilling)

%include openturns/SpaceFilling.hxx
namespace OT { %extend SpaceFilling { SpaceFilling(const SpaceFilling & other) { return new OT::SpaceFilling(other); } } }
