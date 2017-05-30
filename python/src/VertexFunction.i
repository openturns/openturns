// SWIG file VertexFunction.i

%{
#include "openturns/VertexFunction.hxx"
%}

%include VertexFunction_doc.i

%include openturns/VertexFunction.hxx
namespace OT { %extend VertexFunction { VertexFunction(const VertexFunction & other) { return new OT::VertexFunction(other); } } }

