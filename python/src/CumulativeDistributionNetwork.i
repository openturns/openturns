// SWIG file CumulativeDistributionNetwork.i

%{
#include "openturns/CumulativeDistributionNetwork.hxx"
%}

%include CumulativeDistributionNetwork_doc.i

%include openturns/CumulativeDistributionNetwork.hxx

namespace OT { %extend CumulativeDistributionNetwork { CumulativeDistributionNetwork(const CumulativeDistributionNetwork & other) { return new OT::CumulativeDistributionNetwork(other); } } }
