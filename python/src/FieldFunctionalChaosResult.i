// SWIG file FieldFunctionalChaosResult.i

%{
#include "openturns/FieldFunctionalChaosResult.hxx"
%}

%include FieldFunctionalChaosResult_doc.i

%include openturns/FieldFunctionalChaosResult.hxx
namespace OT{ %extend FieldFunctionalChaosResult { FieldFunctionalChaosResult(const FieldFunctionalChaosResult & other) { return new OT::FieldFunctionalChaosResult(other); } } }

OTTypedCollectionInterfaceObjectHelper(KarhunenLoeveResult)
OTDefaultCollectionConvertFunctions(KarhunenLoeveResult)
