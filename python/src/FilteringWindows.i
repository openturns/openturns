// SWIG file FilteringWindows.i

%{
#include "openturns/FilteringWindows.hxx"
%}

%include  FilteringWindows_doc.i

OTTypedInterfaceObjectHelper(FilteringWindows)
 
%include openturns/FilteringWindows.hxx
namespace OT{%extend FilteringWindows { FilteringWindows(const FilteringWindows & other) { return new OT::FilteringWindows(other); } } }
