// SWIG file UniformFactory.i

%{
#include "openturns/UniformFactory.hxx"
%}

%include UniformFactory_doc.i

%include openturns/UniformFactory.hxx
namespace OT { %extend UniformFactory { UniformFactory(const UniformFactory & other) { return new OT::UniformFactory(other); } } }
