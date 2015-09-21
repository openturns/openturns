// SWIG file ProductPolynomialEvaluationImplementation.i

%{
#include "ProductPolynomialEvaluationImplementation.hxx"
%}

%include ProductPolynomialEvaluationImplementation_doc.i

%template(PolynomialCollection) OT::Collection<OT::UniVariatePolynomial>;

%template(PolynomialPersistentCollection) OT::PersistentCollection<OT::UniVariatePolynomial>;

%include ProductPolynomialEvaluationImplementation.hxx
namespace OT { %extend ProductPolynomialEvaluationImplementation { ProductPolynomialEvaluationImplementation(const ProductPolynomialEvaluationImplementation & other) { return new OT::ProductPolynomialEvaluationImplementation(other); } } }
