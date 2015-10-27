// SWIG file DistributionFactoryResult.i

%{
#include "DistributionFactoryResult.hxx"
%}

%include DistributionFactoryResult_doc.i

%include DistributionFactoryResult.hxx
namespace OT { %extend DistributionFactoryResult { DistributionFactoryResult(const DistributionFactoryResult & other) { return new OT::DistributionFactoryResult(other); } } }
