// SWIG file OrthogonalUniVariatePolynomial.i

%{
#include "openturns/OrthogonalUniVariatePolynomial.hxx"
%}

%include OrthogonalUniVariatePolynomial_doc.i

%include openturns/OrthogonalUniVariatePolynomial.hxx
namespace OT { %extend OrthogonalUniVariatePolynomial { OrthogonalUniVariatePolynomial(const OrthogonalUniVariatePolynomial & other) { return new OT::OrthogonalUniVariatePolynomial(other); } } }
