// SWIG file TruncatedDistribution.i

%{
#include "openturns/TruncatedDistribution.hxx"
%}

%include TruncatedDistribution_doc.i

%include openturns/TruncatedDistribution.hxx
namespace OT { %extend TruncatedDistribution { TruncatedDistribution(const TruncatedDistribution & other) { return new OT::TruncatedDistribution(other); } } }
