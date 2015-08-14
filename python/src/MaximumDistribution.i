// SWIG file MaximumDistribution.i

%{
#include "MaximumDistribution.hxx"
%}

%include MaximumDistribution.hxx
namespace OT { %extend MaximumDistribution { MaximumDistribution(const MaximumDistribution & other) { return new OT::MaximumDistribution(other); } } }
