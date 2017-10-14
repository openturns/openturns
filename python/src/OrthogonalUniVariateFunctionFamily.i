// SWIG file OrthogonalUniVariateFunctionFamily.i

%{
#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"
%}

%include OrthogonalUniVariateFunctionFamily_doc.i

OTDefaultCollectionConvertFunctionsMisnamed(OrthogonalUniVariateFunctionFamily, OrthogonalUniVariateFunctionFactory)

OTTypedInterfaceObjectImplementationHelper(OrthogonalUniVariateFunctionFamily, OrthogonalUniVariateFunctionFactory)
OTTypedCollectionInterfaceObjectMisnamedHelper(OrthogonalUniVariateFunctionFamily, FunctionFamilyCollection)

%include openturns/OrthogonalUniVariateFunctionFamily.hxx

namespace OT{  

%extend OrthogonalUniVariateFunctionFamily {

OrthogonalUniVariateFunctionFamily(const OrthogonalUniVariateFunctionFamily & other) { return new OT::OrthogonalUniVariateFunctionFamily(other); }

}

}
