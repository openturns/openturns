// SWIG file SamplerImplementation.i

%{
#include "openturns/SamplerImplementation.hxx"
%}

%include SamplerImplementation_doc.i

%include openturns/SamplerImplementation.hxx
namespace OT { %extend SamplerImplementation { SamplerImplementation(const SamplerImplementation & other) { return new OT::SamplerImplementation(other); } } }
