// SWIG file HaltonSequence.i

%{
#include "HaltonSequence.hxx"
%}

%include HaltonSequence_doc.i

%include HaltonSequence.hxx
namespace OT{ %extend HaltonSequence { HaltonSequence(const HaltonSequence & other) { return new OT::HaltonSequence(other); } } }
