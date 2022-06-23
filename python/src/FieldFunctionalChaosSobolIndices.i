// SWIG file FieldFunctionalChaosSobolIndices.i

%{
#include "openturns/FieldFunctionalChaosSobolIndices.hxx"
%}

%include FieldFunctionalChaosSobolIndices_doc.i

%include openturns/FieldFunctionalChaosSobolIndices.hxx
namespace OT{ %extend FieldFunctionalChaosSobolIndices { FieldFunctionalChaosSobolIndices(const FieldFunctionalChaosSobolIndices & other) { return new OT::FieldFunctionalChaosSobolIndices(other); } } }

