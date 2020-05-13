// SWIG file JansenSobolIndices.i

%{
#include "openturns/JansenSobolIndices.hxx"
%}

%include JansenSobolIndices_doc.i

%include openturns/JansenSobolIndices.hxx
namespace OT{ %extend JansenSobolIndices { JansenSobolIndices(const JansenSobolIndices & other) { return new OT::JansenSobolIndices(other); } } }
