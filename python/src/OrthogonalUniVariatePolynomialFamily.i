// SWIG file OrthogonalUniVariatePolynomialFamily.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weights ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Point OT::OrthogonalUniVariatePolynomialFamily::getNodesAndWeights(OT::Point & weights) const;

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

%clear OT::Point & weights;
