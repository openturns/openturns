// SWIG file Normal.i

%{
#include "Normal.hxx"
%}

%include Normal_doc.i

%include Normal.hxx
namespace OT { %extend Normal { Normal(const Normal & other) { return new OT::Normal(other); } } }
