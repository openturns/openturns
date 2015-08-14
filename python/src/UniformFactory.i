// SWIG file UniformFactory.i

%{
#include "UniformFactory.hxx"
%}

%include UniformFactory_doc.i

%include UniformFactory.hxx
namespace OT { %extend UniformFactory { UniformFactory(const UniformFactory & other) { return new OT::UniformFactory(other); } } }
