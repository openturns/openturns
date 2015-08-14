// SWIG file Uniform.i

%{
#include "Uniform.hxx"
%}

%include Uniform_doc.i

%include Uniform.hxx
namespace OT { %extend Uniform { Uniform(const Uniform & other) { return new OT::Uniform(other); } } }
