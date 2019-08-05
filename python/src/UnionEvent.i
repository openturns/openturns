// SWIG file UnionEvent.i

%{
#include "openturns/IntersectionEvent.hxx"
%}

%include UnionEvent_doc.i

%apply const RandomVectorCollection & { const OT::UnionEvent::RandomVectorCollection & };

%include openturns/UnionEvent.hxx

namespace OT { %extend UnionEvent { UnionEvent(const UnionEvent & other) { return new OT::UnionEvent(other); } } }
