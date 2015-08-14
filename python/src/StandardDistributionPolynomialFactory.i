// SWIG file StandardDistributionPolynomialFactory.i

%{
#include "StandardDistributionPolynomialFactory.hxx"
%}

%include StandardDistributionPolynomialFactory_doc.i

%include StandardDistributionPolynomialFactory.hxx
namespace OT { %extend StandardDistributionPolynomialFactory { StandardDistributionPolynomialFactory(const StandardDistributionPolynomialFactory & other) { return new OT::StandardDistributionPolynomialFactory(other); } } }
