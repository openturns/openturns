// SWIG file OrthogonalUniVariatePolynomialFunctionFactory.i

%{
#include "openturns/OrthogonalUniVariatePolynomialFunctionFactory.hxx"
%}

%include OrthogonalUniVariatePolynomialFunctionFactory_doc.i

%include openturns/OrthogonalUniVariatePolynomialFunctionFactory.hxx
namespace OT { %extend OrthogonalUniVariatePolynomialFunctionFactory { OrthogonalUniVariatePolynomialFunctionFactory(const OrthogonalUniVariatePolynomialFunctionFactory & other) { return new OT::OrthogonalUniVariatePolynomialFunctionFactory(other); } } }
