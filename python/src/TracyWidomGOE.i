// SWIG file TracyWidomGOE.i

%{
#include "TracyWidomGOE.hxx"
%}

%include TracyWidomGOE.hxx
namespace OT { %extend TracyWidomGOE { TracyWidomGOE(const TracyWidomGOE & other) { return new OT::TracyWidomGOE(other); } } }
