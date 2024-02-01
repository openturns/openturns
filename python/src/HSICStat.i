// SWIG file HSICStat.i

%{
#include "openturns/HSICStat.hxx"
%}

OTTypedInterfaceObjectHelper(HSICStat)

%include HSICStat_doc.i

%copyctor OT::HSICStat;

%include openturns/HSICStat.hxx
