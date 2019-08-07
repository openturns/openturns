// SWIG file LeastSquaresDistributionFactory.i

%{
#include "openturns/LeastSquaresDistributionFactory.hxx"
%}

%include LeastSquaresDistributionFactory_doc.i

%ignore OT::LeastSquaresDistributionFactory::buildParameter;

%include openturns/LeastSquaresDistributionFactory.hxx
namespace OT { %extend LeastSquaresDistributionFactory { LeastSquaresDistributionFactory(const LeastSquaresDistributionFactory & other) { return new OT::LeastSquaresDistributionFactory(other); } } }
