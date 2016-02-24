// SWIG file Uniform.i

%{
#include "openturns/Uniform.hxx"
%}

%include Uniform_doc.i

%include openturns/Uniform.hxx
namespace OT { %extend Uniform { Uniform(const Uniform & other) { return new OT::Uniform(other); } } }
