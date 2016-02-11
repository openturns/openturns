// SWIG file FunctionalChaosAlgorithm.i

%{
#include "openturns/FunctionalChaosAlgorithm.hxx"
%}

%include FunctionalChaosAlgorithm_doc.i

%include openturns/FunctionalChaosAlgorithm.hxx

namespace OT{ %extend FunctionalChaosAlgorithm { FunctionalChaosAlgorithm(const FunctionalChaosAlgorithm & other) { return new OT::FunctionalChaosAlgorithm(other); } } }
