// SWIG file UniVariateFunctionFamily.i

%{
#include "openturns/UniVariateFunctionFamily.hxx"
%}

%include UniVariateFunctionFamily_doc.i

OTDefaultCollectionConvertFunctionsMisnamed(UniVariateFunctionFamily, UniVariateFunctionFactory)

OTTypedInterfaceObjectImplementationHelper(UniVariateFunctionFamily, UniVariateFunctionFactory)
OTTypedCollectionInterfaceObjectMisnamedHelper(UniVariateFunctionFamily, FunctionFamilyCollection)

%include openturns/UniVariateFunctionFamily.hxx

%copyctor OT::UniVariateFunctionFamily;
