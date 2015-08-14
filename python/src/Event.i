// SWIG file Event.i

%{
#include "Event.hxx"
%}

%include Event_doc.i

%include Event.hxx
namespace OT { %extend Event { Event(const Event & other) { return new OT::Event(other); } } }
