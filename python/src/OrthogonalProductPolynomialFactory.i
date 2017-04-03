// SWIG file OrthogonalProductPolynomialFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weights ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::OrthogonalProductPolynomialFactory::getNodesAndWeights(const Indices & degrees, OT::Point & weights) const;

%{
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
%}

%include OrthogonalProductPolynomialFactory_doc.i

%include openturns/OrthogonalProductPolynomialFactory.hxx
namespace OT{ %extend OrthogonalProductPolynomialFactory { OrthogonalProductPolynomialFactory(const OrthogonalProductPolynomialFactory & other) { return new OT::OrthogonalProductPolynomialFactory(other); } } }

%clear OT::Point & weights;

