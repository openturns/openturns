// SWIG file KPermutationsDistribution.i

%{
#include "openturns/KPermutationsDistribution.hxx"
%}

%include KPermutationsDistribution_doc.i

%include openturns/KPermutationsDistribution.hxx
namespace OT { %extend KPermutationsDistribution { KPermutationsDistribution(const KPermutationsDistribution & other) { return new OT::KPermutationsDistribution(other); } } }
