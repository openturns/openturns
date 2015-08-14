// SWIG file KPermutationsDistribution.i

%{
#include "KPermutationsDistribution.hxx"
%}

%include KPermutationsDistribution_doc.i

%include KPermutationsDistribution.hxx
namespace OT { %extend KPermutationsDistribution { KPermutationsDistribution(const KPermutationsDistribution & other) { return new OT::KPermutationsDistribution(other); } } }
