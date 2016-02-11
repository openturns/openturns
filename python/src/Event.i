// SWIG file Event.i

%{
#include "openturns/Event.hxx"
%}

%include Event_doc.i

%include openturns/Event.hxx
namespace OT { %extend Event { Event(const Event & other) { return new OT::Event(other); } } }
