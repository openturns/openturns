// SWIG file HaltonSequence.i

%{
#include "openturns/HaltonSequence.hxx"
%}

%include HaltonSequence_doc.i

%include openturns/HaltonSequence.hxx
namespace OT{ %extend HaltonSequence { HaltonSequence(const HaltonSequence & other) { return new OT::HaltonSequence(other); } } }
