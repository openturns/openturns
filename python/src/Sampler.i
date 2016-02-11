// SWIG file Sampler.i

%{
#include "openturns/Sampler.hxx"
%}

%include Sampler_doc.i

OTTypedInterfaceObjectHelper(Sampler)

%include openturns/Sampler.hxx
namespace OT {  
%extend Sampler {
  const String __str__() const { return self->__repr__(); }
}
}
namespace OT { %extend Sampler { Sampler(const Sampler & other) { return new OT::Sampler(other); } } }
