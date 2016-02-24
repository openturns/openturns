// SWIG file MaximumLikelihoodFactory.i

%{
#include "openturns/MaximumLikelihoodFactory.hxx"
%}

%include MaximumLikelihoodFactory_doc.i

%ignore OT::MaximumLikelihoodFactory::setParallel;
%ignore OT::MaximumLikelihoodFactory::buildParameter;

%include openturns/MaximumLikelihoodFactory.hxx
namespace OT { %extend MaximumLikelihoodFactory { MaximumLikelihoodFactory(const MaximumLikelihoodFactory & other) { return new OT::MaximumLikelihoodFactory(other); } } }
