// SWIG file TracyWidomGOE.i

%{
#include "openturns/TracyWidomGOE.hxx"
%}

%include openturns/TracyWidomGOE.hxx
namespace OT { %extend TracyWidomGOE { TracyWidomGOE(const TracyWidomGOE & other) { return new OT::TracyWidomGOE(other); } } }
