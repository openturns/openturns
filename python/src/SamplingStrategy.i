// SWIG file SamplingStrategy.i

%{
#include "openturns/SamplingStrategy.hxx"
%}

%include SamplingStrategy_doc.i

OTTypedInterfaceObjectHelper(SamplingStrategy)

%include openturns/SamplingStrategy.hxx
namespace OT{ %extend SamplingStrategy { SamplingStrategy(const SamplingStrategy & other) { return new OT::SamplingStrategy(other); } } }
