// SWIG file Box.i

%{
#include "Box.hxx"
%}

%include Box_doc.i

%include Box.hxx
namespace OT { %extend Box { Box(const Box & other) { return new OT::Box(other); } } }
