// SWIG file FilteringWindows.i

%{
#include "FilteringWindows.hxx"
%}

%include  FilteringWindows_doc.i

OTTypedInterfaceObjectHelper(FilteringWindows)
 
%include FilteringWindows.hxx
namespace OT{%extend FilteringWindows { FilteringWindows(const FilteringWindows & other) { return new OT::FilteringWindows(other); } } }
