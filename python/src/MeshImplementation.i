// SWIG file MeshImplementation.i

%{
#include "openturns/MeshImplementation.hxx"
%}

%include MeshImplementation_doc.i

%include openturns/MeshImplementation.hxx
namespace OT { %extend MeshImplementation { MeshImplementation(const MeshImplementation & other) { return new OT::MeshImplementation(other); } } }
