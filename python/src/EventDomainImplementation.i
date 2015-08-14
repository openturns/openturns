// SWIG file EventDomainImplementation.i

%{
#include "EventDomainImplementation.hxx"
%}

%include EventDomainImplementation.hxx
namespace OT { %extend EventDomainImplementation { EventDomainImplementation(const EventDomainImplementation & other) { return new OT::EventDomainImplementation(other); } } }
