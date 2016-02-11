// SWIG file RandomWalk.i

%{
#include "openturns/RandomWalk.hxx"
%}

%include RandomWalk_doc.i

%include openturns/RandomWalk.hxx
namespace OT { %extend RandomWalk { RandomWalk(const RandomWalk & other) { return new OT::RandomWalk(other); } } }
