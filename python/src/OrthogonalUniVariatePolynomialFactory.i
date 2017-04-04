// SWIG file OrthogonalUniVariatePolynomialFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weights ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Point OT::OrthogonalUniVariatePolynomialFactory::getNodesAndWeights(const OT::UnsignedInteger n, OT::Point & weights) const;

%{
#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
%}

%include OrthogonalUniVariatePolynomialFactory_doc.i

%include openturns/OrthogonalUniVariatePolynomialFactory.hxx
namespace OT{ %extend OrthogonalUniVariatePolynomialFactory { OrthogonalUniVariatePolynomialFactory(const OrthogonalUniVariatePolynomialFactory & other) { return new OT::OrthogonalUniVariatePolynomialFactory(other); } } }

%clear OT::Point & weights;

