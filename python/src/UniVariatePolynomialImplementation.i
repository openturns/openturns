// SWIG file UniVariatePolynomialImplementation.i

%{
#include "UniVariatePolynomialImplementation.hxx"
%}

%include UniVariatePolynomialImplementation_doc.i

%include UniVariatePolynomialImplementation.hxx
namespace OT { %extend UniVariatePolynomialImplementation { UniVariatePolynomialImplementation(const UniVariatePolynomialImplementation & other) { return new OT::UniVariatePolynomialImplementation(other); } } }
