// SWIG file RandomWalk.i

%{
#include "RandomWalk.hxx"
%}

%include RandomWalk_doc.i

%include RandomWalk.hxx
namespace OT { %extend RandomWalk { RandomWalk(const RandomWalk & other) { return new OT::RandomWalk(other); } } }
