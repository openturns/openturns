// SWIG file StandardDistributionPolynomialFactory.i

%{
#include "openturns/StandardDistributionPolynomialFactory.hxx"
%}

%include StandardDistributionPolynomialFactory_doc.i

%include openturns/StandardDistributionPolynomialFactory.hxx
namespace OT { %extend StandardDistributionPolynomialFactory { StandardDistributionPolynomialFactory(const StandardDistributionPolynomialFactory & other) { return new OT::StandardDistributionPolynomialFactory(other); } } }
