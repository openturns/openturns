// SWIG file MarginalDistribution.i

%{
#include "MarginalDistribution.hxx"
%}

%include MarginalDistribution.hxx
namespace OT { %extend MarginalDistribution { MarginalDistribution(const MarginalDistribution & other) { return new OT::MarginalDistribution(other); } } }
