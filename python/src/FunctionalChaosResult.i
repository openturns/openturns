// SWIG file FunctionalChaosResult.i

%{
#include "openturns/FunctionalChaosResult.hxx"
%}

%include FunctionalChaosResult_doc.i

%include openturns/FunctionalChaosResult.hxx

namespace OT{ %extend FunctionalChaosResult { FunctionalChaosResult(const FunctionalChaosResult & other) { return new OT::FunctionalChaosResult(other); } } }
