// SWIG file MauntzKucherenkoSobolIndices.i

%{
#include "openturns/MauntzKucherenkoSobolIndices.hxx"
%}

%include MauntzKucherenkoSobolIndices_doc.i

%include openturns/MauntzKucherenkoSobolIndices.hxx
namespace OT{ %extend MauntzKucherenkoSobolIndices { MauntzKucherenkoSobolIndices(const MauntzKucherenkoSobolIndices & other) { return new OT::MauntzKucherenkoSobolIndices(other); } } }
