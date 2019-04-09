// SWIG file Equal.i

%{
#include "openturns/Equal.hxx"
%}

%include Equal_doc.i

%include openturns/Equal.hxx

namespace OT { %extend Equal { Equal(const Equal & other) { return new OT::Equal(other); } } }
