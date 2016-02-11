// SWIG file OrthogonalUniVariatePolynomialFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & weights ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalPoint OT::OrthogonalUniVariatePolynomialFactory::getNodesAndWeights(const OT::UnsignedInteger n, OT::NumericalPoint & weights) const;

%{
#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
%}

%include OrthogonalUniVariatePolynomialFactory_doc.i

%include openturns/OrthogonalUniVariatePolynomialFactory.hxx
namespace OT{ %extend OrthogonalUniVariatePolynomialFactory { OrthogonalUniVariatePolynomialFactory(const OrthogonalUniVariatePolynomialFactory & other) { return new OT::OrthogonalUniVariatePolynomialFactory(other); } } }

%clear OT::NumericalPoint & weights;

