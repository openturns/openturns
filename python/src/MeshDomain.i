// SWIG file MeshDomain.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/MeshDomain.hxx"
%}

%include MeshDomain_doc.i

%include openturns/MeshDomain.hxx

namespace OT { %extend MeshDomain { MeshDomain(const MeshDomain & other) { return new OT::MeshDomain(other); } } }
