// SWIG file ProductPolynomialEvaluation.i

%{
#include "openturns/ProductPolynomialEvaluation.hxx"
%}

%include ProductPolynomialEvaluation_doc.i

%template(PolynomialCollection) OT::Collection<OT::UniVariatePolynomial>;
%template(_PolynomialPersistentCollection) OT::PersistentCollection<OT::UniVariatePolynomial>;

%copyctor OT::ProductPolynomialEvaluation;

%include openturns/ProductPolynomialEvaluation.hxx
