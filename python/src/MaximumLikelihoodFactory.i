// SWIG file MaximumLikelihoodFactory.i

%{
#include "openturns/MaximumLikelihoodFactory.hxx"
%}

%include MaximumLikelihoodFactory_doc.i

%ignore OT::MaximumLikelihoodFactory::buildParameter;
%ignore OT::MaximumLikelihoodFactory::BuildMaximumLikelihoodEstimator;

%include openturns/MaximumLikelihoodFactory.hxx
namespace OT { %extend MaximumLikelihoodFactory { MaximumLikelihoodFactory(const MaximumLikelihoodFactory & other) { return new OT::MaximumLikelihoodFactory(other); } } }
