// SWIG file SamplingStrategyImplementation.i

%{
#include "SamplingStrategyImplementation.hxx"
%}

%include SamplingStrategyImplementation_doc.i

%include SamplingStrategyImplementation.hxx
namespace OT{ %extend SamplingStrategyImplementation { SamplingStrategyImplementation(const SamplingStrategyImplementation & other) { return new OT::SamplingStrategyImplementation(other); } } }
