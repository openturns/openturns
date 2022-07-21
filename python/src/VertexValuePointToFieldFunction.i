// SWIG file VertexValuePointToFieldFunction.i

%{
#include "openturns/VertexValuePointToFieldFunction.hxx"
%}

%include VertexValuePointToFieldFunction_doc.i

%include openturns/VertexValuePointToFieldFunction.hxx

namespace OT {
%extend VertexValuePointToFieldFunction {

VertexValuePointToFieldFunction(const VertexValuePointToFieldFunction & other) { return new OT::VertexValuePointToFieldFunction(other); }

}}

