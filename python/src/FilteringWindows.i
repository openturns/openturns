// SWIG file FilteringWindows.i

%{
#include "openturns/FilteringWindows.hxx"
%}

%include  FilteringWindows_doc.i

OTTypedInterfaceObjectHelper(FilteringWindows)
 
%copyctor OT::FilteringWindows;

%include openturns/FilteringWindows.hxx
