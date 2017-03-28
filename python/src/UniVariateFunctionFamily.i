// SWIG file UniVariateFunctionFamily.i

%{
#include "openturns/UniVariateFunctionFamily.hxx"
%}

%include UniVariateFunctionFamily_doc.i

OTDefaultCollectionConvertFunctionsMisnamed(UniVariateFunctionFamily, UniVariateFunctionFactory)

OTTypedInterfaceObjectImplementationHelper(UniVariateFunctionFamily, UniVariateFunctionFactory)
OTTypedCollectionInterfaceObjectMisnamedHelper(UniVariateFunctionFamily, FunctionFamilyCollection)

%include openturns/UniVariateFunctionFamily.hxx

namespace OT{

%extend UniVariateFunctionFamily {

UniVariateFunctionFamily(const UniVariateFunctionFamily & other) { return new OT::UniVariateFunctionFamily(other); }

}

}
