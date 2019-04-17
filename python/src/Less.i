// SWIG file Less.i

%{
#include "openturns/Less.hxx"
%}

%include Less_doc.i

%include openturns/Less.hxx

namespace OT { %extend Less { Less(const Less & other) { return new OT::Less(other); } } }
