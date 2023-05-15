// SWIG file DistributionFactoryLikelihoodResult.i

%{
#include "openturns/DistributionFactoryLikelihoodResult.hxx"
%}

%include DistributionFactoryLikelihoodResult_doc.i

%include openturns/DistributionFactoryLikelihoodResult.hxx
namespace OT { %extend DistributionFactoryLikelihoodResult { DistributionFactoryLikelihoodResult(const DistributionFactoryLikelihoodResult & other) { return new OT::DistributionFactoryLikelihoodResult(other); } } }
