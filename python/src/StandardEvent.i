// SWIG file StandardEvent.i

%{
#include "StandardEvent.hxx"
%}

%include StandardEvent_doc.i

%include StandardEvent.hxx
namespace OT { %extend StandardEvent { StandardEvent(const StandardEvent & other) { return new OT::StandardEvent(other); } } }
