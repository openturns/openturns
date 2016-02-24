// SWIG file MaximumEntropyOrderStatisticsDistribution.i

%{
#include "openturns/MaximumEntropyOrderStatisticsDistribution.hxx"
%}

%include MaximumEntropyOrderStatisticsDistribution_doc.i

%include openturns/MaximumEntropyOrderStatisticsDistribution.hxx
namespace OT { %extend MaximumEntropyOrderStatisticsDistribution { MaximumEntropyOrderStatisticsDistribution(const MaximumEntropyOrderStatisticsDistribution & other) { return new OT::MaximumEntropyOrderStatisticsDistribution(other); } } }
