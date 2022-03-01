// SWIG file HSICStat.i

%{
#include "openturns/HSICStat.hxx"
%}

OTTypedInterfaceObjectHelper(HSICStat)
//%OTTypedCollectionInterfaceObjectHelper(HSICStat)

%include HSICStat_doc.i

%include openturns/HSICStat.hxx
namespace OT{ %extend HSICStat { HSICStat(const HSICStat & other) { return new OT::HSICStat(other); } } }
