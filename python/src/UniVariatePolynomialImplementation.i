// SWIG file UniVariatePolynomialImplementation.i

%{
#include "openturns/UniVariatePolynomialImplementation.hxx"
%}

%include UniVariatePolynomialImplementation_doc.i

%include openturns/UniVariatePolynomialImplementation.hxx
namespace OT { %extend UniVariatePolynomialImplementation { UniVariatePolynomialImplementation(const UniVariatePolynomialImplementation & other) { return new OT::UniVariatePolynomialImplementation(other); } } }
