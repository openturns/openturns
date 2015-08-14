// SWIG file TruncatedDistribution.i

%{
#include "TruncatedDistribution.hxx"
%}

%include TruncatedDistribution_doc.i

%include TruncatedDistribution.hxx
namespace OT { %extend TruncatedDistribution { TruncatedDistribution(const TruncatedDistribution & other) { return new OT::TruncatedDistribution(other); } } }
