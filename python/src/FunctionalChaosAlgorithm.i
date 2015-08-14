// SWIG file FunctionalChaosAlgorithm.i

%{
#include "FunctionalChaosAlgorithm.hxx"
%}

%include FunctionalChaosAlgorithm_doc.i

%include FunctionalChaosAlgorithm.hxx

namespace OT{ %extend FunctionalChaosAlgorithm { FunctionalChaosAlgorithm(const FunctionalChaosAlgorithm & other) { return new OT::FunctionalChaosAlgorithm(other); } } }
