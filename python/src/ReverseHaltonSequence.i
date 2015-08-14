// SWIG file ReverseHaltonSequence.i

%{
#include "ReverseHaltonSequence.hxx"
%}

%include ReverseHaltonSequence_doc.i

%include ReverseHaltonSequence.hxx
namespace OT{ %extend ReverseHaltonSequence { ReverseHaltonSequence(const ReverseHaltonSequence & other) { return new OT::ReverseHaltonSequence(other); } } }
