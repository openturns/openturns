// SWIG file MaximumLikelihoodFactory.i

%{
#include "openturns/MaximumLikelihoodFactory.hxx"
%}

%include MaximumLikelihoodFactory_doc.i

%ignore OT::MaximumLikelihoodFactory::buildParameter;
%ignore OT::MaximumLikelihoodFactory::BuildMaximumLikelihoodEstimator;

%copyctor OT::MaximumLikelihoodFactory;

%include openturns/MaximumLikelihoodFactory.hxx
