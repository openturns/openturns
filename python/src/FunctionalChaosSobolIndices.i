// SWIG file FunctionalChaosSobolIndices.i

%{
#include "openturns/FunctionalChaosSobolIndices.hxx"
%}

%include FunctionalChaosSobolIndices_doc.i

%include openturns/FunctionalChaosSobolIndices.hxx
namespace OT { %extend FunctionalChaosSobolIndices { FunctionalChaosSobolIndices(const FunctionalChaosSobolIndices & other) { return new OT::FunctionalChaosSobolIndices(other); } } }
