// SWIG file Box.i

%{
#include "openturns/Box.hxx"
%}

%include Box_doc.i

%include openturns/Box.hxx
namespace OT { %extend Box { Box(const Box & other) { return new OT::Box(other); } } }
