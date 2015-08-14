// SWIG file OrthogonalUniVariatePolynomial.i

%{
#include "OrthogonalUniVariatePolynomial.hxx"
%}

%include OrthogonalUniVariatePolynomial_doc.i

%include OrthogonalUniVariatePolynomial.hxx
namespace OT { %extend OrthogonalUniVariatePolynomial { OrthogonalUniVariatePolynomial(const OrthogonalUniVariatePolynomial & other) { return new OT::OrthogonalUniVariatePolynomial(other); } } }
