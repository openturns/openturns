// SWIG file SamplerImplementation.i

%{
#include "SamplerImplementation.hxx"
%}

%include SamplerImplementation_doc.i

%include SamplerImplementation.hxx
namespace OT { %extend SamplerImplementation { SamplerImplementation(const SamplerImplementation & other) { return new OT::SamplerImplementation(other); } } }
