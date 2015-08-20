// SWIG file SamplingStrategy.i

%{
#include "SamplingStrategy.hxx"
%}

%include SamplingStrategy_doc.i

OTTypedInterfaceObjectHelper(SamplingStrategy)

%include SamplingStrategy.hxx
namespace OT{ %extend SamplingStrategy { SamplingStrategy(const SamplingStrategy & other) { return new OT::SamplingStrategy(other); } } }
