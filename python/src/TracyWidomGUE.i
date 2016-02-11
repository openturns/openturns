// SWIG file TracyWidomGUE.i

%{
#include "openturns/TracyWidomGUE.hxx"
%}

%include openturns/TracyWidomGUE.hxx
namespace OT { %extend TracyWidomGUE { TracyWidomGUE(const TracyWidomGUE & other) { return new OT::TracyWidomGUE(other); } } }
