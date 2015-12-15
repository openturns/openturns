// SWIG file CumulativeDistributionNetwork.i

%{
#include "CumulativeDistributionNetwork.hxx"
%}

%include CumulativeDistributionNetwork_doc.i

%include CumulativeDistributionNetwork.hxx

namespace OT { %extend CumulativeDistributionNetwork { CumulativeDistributionNetwork(const CumulativeDistributionNetwork & other) { return new OT::CumulativeDistributionNetwork(other); } } }
