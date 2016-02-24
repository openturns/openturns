// SWIG file DistributionFactoryResult.i

%{
#include "openturns/DistributionFactoryResult.hxx"
%}

%include DistributionFactoryResult_doc.i

%include openturns/DistributionFactoryResult.hxx
namespace OT { %extend DistributionFactoryResult { DistributionFactoryResult(const DistributionFactoryResult & other) { return new OT::DistributionFactoryResult(other); } } }
