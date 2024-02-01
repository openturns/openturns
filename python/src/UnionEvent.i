// SWIG file UnionEvent.i

%{
#include "openturns/IntersectionEvent.hxx"
%}

%include UnionEvent_doc.i

%apply const RandomVectorCollection & { const OT::UnionEvent::RandomVectorCollection & };

%copyctor OT::UnionEvent;

%include openturns/UnionEvent.hxx
