// SWIG file TracyWidomGUE.i

%{
#include "TracyWidomGUE.hxx"
%}

%include TracyWidomGUE.hxx
namespace OT { %extend TracyWidomGUE { TracyWidomGUE(const TracyWidomGUE & other) { return new OT::TracyWidomGUE(other); } } }
