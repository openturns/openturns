// SWIG file MaximumEntropyOrderStatisticsDistribution.i

%{
#include "MaximumEntropyOrderStatisticsDistribution.hxx"
%}

%include MaximumEntropyOrderStatisticsDistribution_doc.i

%include MaximumEntropyOrderStatisticsDistribution.hxx
namespace OT { %extend MaximumEntropyOrderStatisticsDistribution { MaximumEntropyOrderStatisticsDistribution(const MaximumEntropyOrderStatisticsDistribution & other) { return new OT::MaximumEntropyOrderStatisticsDistribution(other); } } }
