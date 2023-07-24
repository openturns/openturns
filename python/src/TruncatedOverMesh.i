// SWIG file TruncatedOverMesh.i

%{
#include "openturns/TruncatedOverMesh.hxx"
%}

%include TruncatedOverMesh_doc.i

%include openturns/TruncatedOverMesh.hxx
namespace OT { %extend TruncatedOverMesh { TruncatedOverMesh(const TruncatedOverMesh & other) { return new OT::TruncatedOverMesh(other); } } }
