// SWIG file MartinezSobolIndices.i

%{
#include "openturns/MartinezSobolIndices.hxx"
%}

%include MartinezSobolIndices_doc.i

%include openturns/MartinezSobolIndices.hxx
namespace OT{ %extend MartinezSobolIndices { MartinezSobolIndices(const MartinezSobolIndices & other) { return new OT::MartinezSobolIndices(other); } } }
