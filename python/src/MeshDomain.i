// SWIG file MeshDomain.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/MeshDomain.hxx"
%}

%include MeshDomain_doc.i

%copyctor OT::MeshDomain;

%include openturns/MeshDomain.hxx
