// SWIG file FieldToPointFunctionalChaosAlgorithm.i

%{
#include "openturns/FieldToPointFunctionalChaosAlgorithm.hxx"
%}

%include FieldToPointFunctionalChaosAlgorithm_doc.i

%include openturns/FieldToPointFunctionalChaosAlgorithm.hxx
namespace OT{ %extend FieldToPointFunctionalChaosAlgorithm { FieldToPointFunctionalChaosAlgorithm(const FieldToPointFunctionalChaosAlgorithm & other) { return new OT::FieldToPointFunctionalChaosAlgorithm(other); } } }
