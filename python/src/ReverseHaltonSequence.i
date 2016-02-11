// SWIG file ReverseHaltonSequence.i

%{
#include "openturns/ReverseHaltonSequence.hxx"
%}

%include ReverseHaltonSequence_doc.i

%include openturns/ReverseHaltonSequence.hxx
namespace OT{ %extend ReverseHaltonSequence { ReverseHaltonSequence(const ReverseHaltonSequence & other) { return new OT::ReverseHaltonSequence(other); } } }
