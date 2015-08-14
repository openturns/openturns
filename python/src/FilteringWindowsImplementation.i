// SWIG file FilteringWindowsImplementation.i

%{
#include "FilteringWindowsImplementation.hxx"
%}

%include FilteringWindowsImplementation_doc.i

%include FilteringWindowsImplementation.hxx
namespace OT{%extend FilteringWindowsImplementation { FilteringWindowsImplementation(const FilteringWindowsImplementation & other) { return new OT::FilteringWindowsImplementation(other); } } }
