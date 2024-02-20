// SWIG file FieldFunctionalChaosResult.i

%{
#include "openturns/FieldFunctionalChaosResult.hxx"
%}

%include FieldFunctionalChaosResult_doc.i

%copyctor OT::FieldFunctionalChaosResult;

%include openturns/FieldFunctionalChaosResult.hxx

OTTypedCollectionInterfaceObjectHelper(KarhunenLoeveResult)
OTDefaultCollectionConvertFunctions(KarhunenLoeveResult)
