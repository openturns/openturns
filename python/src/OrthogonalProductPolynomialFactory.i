// SWIG file OrthogonalProductPolynomialFactory.i

%{
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
%}

%include OrthogonalProductPolynomialFactory_doc.i

%include openturns/OrthogonalProductPolynomialFactory.hxx
namespace OT{ %extend OrthogonalProductPolynomialFactory { OrthogonalProductPolynomialFactory(const OrthogonalProductPolynomialFactory & other) { return new OT::OrthogonalProductPolynomialFactory(other); } } }
