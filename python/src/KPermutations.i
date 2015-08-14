// SWIG file KPermutations.i

%{
#include "KPermutations.hxx"
%}

%include KPermutations.hxx
namespace OT { %extend KPermutations { KPermutations(const KPermutations & other) { return new OT::KPermutations(other); } } }
