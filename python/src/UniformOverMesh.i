// SWIG file UniformOverMesh.i

%{
#include "openturns/UniformOverMesh.hxx"
%}

%include UniformOverMesh_doc.i

%include openturns/UniformOverMesh.hxx
namespace OT { %extend UniformOverMesh { UniformOverMesh(const UniformOverMesh & other) { return new OT::UniformOverMesh(other); } } }
