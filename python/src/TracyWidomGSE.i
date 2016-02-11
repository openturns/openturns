// SWIG file TracyWidomGSE.i

%{
#include "openturns/TracyWidomGSE.hxx"
%}

%include openturns/TracyWidomGSE.hxx
namespace OT { %extend TracyWidomGSE { TracyWidomGSE(const TracyWidomGSE & other) { return new OT::TracyWidomGSE(other); } } }
