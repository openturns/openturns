// SWIG file SamplingStrategyImplementation.i

%{
#include "openturns/SamplingStrategyImplementation.hxx"
%}

%include SamplingStrategyImplementation_doc.i

%include openturns/SamplingStrategyImplementation.hxx
namespace OT{ %extend SamplingStrategyImplementation { SamplingStrategyImplementation(const SamplingStrategyImplementation & other) { return new OT::SamplingStrategyImplementation(other); } } }
