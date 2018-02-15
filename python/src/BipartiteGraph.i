// SWIG file BipartiteGraph.i

%{
#include "openturns/BipartiteGraph.hxx"
%}

%include BipartiteGraph_doc.i

%include openturns/BipartiteGraph.hxx
namespace OT { %extend BipartiteGraph { BipartiteGraph(const BipartiteGraph & other) { return new OT::BipartiteGraph(other); } } }
