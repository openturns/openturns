// SWIG file OrthogonalProductPolynomialFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & weights ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalSample OT::OrthogonalProductPolynomialFactory::getNodesAndWeights(const Indices & degrees, OT::NumericalPoint & weights) const;

%{
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
%}

%include OrthogonalProductPolynomialFactory_doc.i

%include openturns/OrthogonalProductPolynomialFactory.hxx
namespace OT{ %extend OrthogonalProductPolynomialFactory { OrthogonalProductPolynomialFactory(const OrthogonalProductPolynomialFactory & other) { return new OT::OrthogonalProductPolynomialFactory(other); } } }

%clear OT::NumericalPoint & weights;

