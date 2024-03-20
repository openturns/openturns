// SWIG file FieldFunctionalChaosResult.i

%{
#include "openturns/FieldFunctionalChaosResult.hxx"
%}

%include FieldFunctionalChaosResult_doc.i

%template(_IndicesCollection2) OT::Collection<OT::Indices>;

%copyctor OT::FieldFunctionalChaosResult;

%include openturns/FieldFunctionalChaosResult.hxx

OTTypedCollectionInterfaceObjectHelper(KarhunenLoeveResult)
OTDefaultCollectionConvertFunctions(KarhunenLoeveResult)
