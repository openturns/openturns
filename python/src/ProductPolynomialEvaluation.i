// SWIG file ProductPolynomialEvaluation.i

%{
#include "openturns/ProductPolynomialEvaluation.hxx"
%}

%include ProductPolynomialEvaluation_doc.i

%template(PolynomialCollection) OT::Collection<OT::UniVariatePolynomial>;
%template(_PolynomialPersistentCollection) OT::PersistentCollection<OT::UniVariatePolynomial>;

%include openturns/ProductPolynomialEvaluation.hxx
namespace OT { %extend ProductPolynomialEvaluation { ProductPolynomialEvaluation(const ProductPolynomialEvaluation & other) { return new OT::ProductPolynomialEvaluation(other); } } }
