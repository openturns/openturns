// SWIG file EventDomain.i

%{
#include "openturns/EventDomain.hxx"
%}

%include EventDomain_doc.i

%include openturns/EventDomain.hxx
namespace OT { %extend EventDomain { EventDomain(const EventDomain & other) { return new OT::EventDomain(other); } } }
