// SWIG file VertexValueFunction.i

%{
#include "openturns/VertexValueFunction.hxx"
%}

%include VertexValueFunction_doc.i

%include openturns/VertexValueFunction.hxx
namespace OT { %extend VertexValueFunction { VertexValueFunction(const VertexValueFunction & other) { return new OT::VertexValueFunction(other); } } }

