// SWIG file LeastSquaresDistributionFactory.i

%{
#include "openturns/LeastSquaresDistributionFactory.hxx"
%}

%include LeastSquaresDistributionFactory_doc.i

%ignore OT::LeastSquaresDistributionFactory::buildParameter;

%copyctor OT::LeastSquaresDistributionFactory;

%include openturns/LeastSquaresDistributionFactory.hxx
