// SWIG file MarginalDistribution.i

%{
#include "openturns/MarginalDistribution.hxx"
%}

%include openturns/MarginalDistribution.hxx
namespace OT { %extend MarginalDistribution { MarginalDistribution(const MarginalDistribution & other) { return new OT::MarginalDistribution(other); } } }
