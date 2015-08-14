// SWIG file LAR.i

%{
#include "LAR.hxx"
%}

%include LAR_doc.i

%include LAR.hxx
namespace OT { %extend LAR { LAR(const LAR & other) { return new OT::LAR(other); } } }
