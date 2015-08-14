// SWIG file FunctionalChaosResult.i

%{
#include "FunctionalChaosResult.hxx"
%}

%include FunctionalChaosResult_doc.i

%include FunctionalChaosResult.hxx

namespace OT{ %extend FunctionalChaosResult { FunctionalChaosResult(const FunctionalChaosResult & other) { return new OT::FunctionalChaosResult(other); } } }
