// SWIG file Text.i

%{
#include "openturns/Text.hxx"
%}

%include Text_doc.i

%include openturns/Text.hxx
namespace OT { %extend Text { Text(const Text & other) { return new OT::Text(other); } } }
