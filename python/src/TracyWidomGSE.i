// SWIG file TracyWidomGSE.i

%{
#include "TracyWidomGSE.hxx"
%}

%include TracyWidomGSE.hxx
namespace OT { %extend TracyWidomGSE { TracyWidomGSE(const TracyWidomGSE & other) { return new OT::TracyWidomGSE(other); } } }
