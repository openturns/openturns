// SWIG file FilteringWindowsImplementation.i

%{
#include "openturns/FilteringWindowsImplementation.hxx"
%}

%include FilteringWindowsImplementation_doc.i

%include openturns/FilteringWindowsImplementation.hxx
namespace OT{%extend FilteringWindowsImplementation { FilteringWindowsImplementation(const FilteringWindowsImplementation & other) { return new OT::FilteringWindowsImplementation(other); } } }
