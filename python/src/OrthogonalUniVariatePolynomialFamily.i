// SWIG file OrthogonalUniVariatePolynomialFamily.i

%{
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"
%}

%include OrthogonalUniVariatePolynomialFamily_doc.i

OTDefaultCollectionConvertFunctionsMisnamed(OrthogonalUniVariatePolynomialFamily, OrthogonalUniVariatePolynomialFactory)

OTTypedInterfaceObjectImplementationHelper(OrthogonalUniVariatePolynomialFamily, OrthogonalUniVariatePolynomialFactory)
OTTypedCollectionInterfaceObjectMisnamedHelper(OrthogonalUniVariatePolynomialFamily, PolynomialFamilyCollection)

%include openturns/OrthogonalUniVariatePolynomialFamily.hxx

namespace OT{  

%extend OrthogonalUniVariatePolynomialFamily {

OrthogonalUniVariatePolynomialFamily(const OrthogonalUniVariatePolynomialFamily & other) { return new OT::OrthogonalUniVariatePolynomialFamily(other); }

}

}
