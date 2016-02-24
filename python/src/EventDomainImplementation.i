// SWIG file EventDomainImplementation.i

%{
#include "openturns/EventDomainImplementation.hxx"
%}

%include openturns/EventDomainImplementation.hxx
namespace OT { %extend EventDomainImplementation { EventDomainImplementation(const EventDomainImplementation & other) { return new OT::EventDomainImplementation(other); } } }
