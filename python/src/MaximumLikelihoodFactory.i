// SWIG file MaximumLikelihoodFactory.i

%{
#include "MaximumLikelihoodFactory.hxx"
%}

%include MaximumLikelihoodFactory_doc.i

%ignore OT::MaximumLikelihoodFactory::setParallel;
%ignore OT::MaximumLikelihoodFactory::buildParameter;

%include MaximumLikelihoodFactory.hxx
namespace OT { %extend MaximumLikelihoodFactory { MaximumLikelihoodFactory(const MaximumLikelihoodFactory & other) { return new OT::MaximumLikelihoodFactory(other); } } }
