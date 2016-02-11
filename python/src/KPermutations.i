// SWIG file KPermutations.i

%{
#include "openturns/KPermutations.hxx"
%}

%include KPermutations_doc.i

%include openturns/KPermutations.hxx
namespace OT { %extend KPermutations { KPermutations(const KPermutations & other) { return new OT::KPermutations(other); } } }
